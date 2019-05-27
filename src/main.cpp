#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

#include <signal.h>
#include <sys/epoll.h>
#include <sys/poll.h>
#include <unistd.h>

#include <opencv2/opencv.hpp>

#include "render.hpp"
#include "v4l2capture.hpp"

static volatile bool keepRunning = true;

int main()
{
    int epoll_fd;
    int cameraNum = 4;
    Render render;
    signal(SIGINT, [](int){ keepRunning = false; });
    std::vector<V4l2Capture> captures(4);
    std::vector<std::array<V4l2Capture::Buffer, 4>> buffers(4);
    std::vector<std::array<void *, 4>> renderBufs(4);

    epoll_fd = epoll_create1(0);
    struct pollfd pfd[4];

    try {
        render.init();
        for (size_t i = 0; i < captures.size(); i++) {
            render.getBufferAddrs(i, renderBufs[i]);
            for (size_t j = 0; j < renderBufs[i].size(); j++) {
                buffers[i][j].start = renderBufs[i][j];
                buffers[i][j].length = 1280 * 800 * 4;
            }
            captures[i].open("/dev/video" + std::to_string(i),
                             V4l2Capture::ImgFormat(
                                 1280, 800, V4l2Capture::PixFormat::XBGR32),
                             buffers[i]);
            captures[i].start();

            // struct epoll_event event = {};
            // event.data.fd = captures[i].getFd();
            // event.events = EPOLLIN | EPOLLET;
            // int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, captures[i].getFd(),
                    // &event);
            // if (ret == -1) {
                // throw std::runtime_error("EPOLL_CTL_ADD error");
            // }
            pfd[i].fd = captures[i].getFd();
            pfd[i].events = POLLIN;
        }

        int frameCount = 0;
        double previousTime = glfwGetTime();
        double currentTime;
        int fCount = 0;

        int index[4] = {0, 0, 0, 0};

        // struct epoll_event *events = static_cast<struct epoll_event *>(calloc(10, sizeof(struct epoll_event)));
        // int nfd;

        while (keepRunning) {
            glfwPollEvents();

            // nfd = epoll_wait(epoll_fd, events, 10, -1);
            // if (nfd == -1)
                // throw std::runtime_error("epoll_wait error, erron: " + std::to_string(errno));

            // for (int i = 0; i < nfd; i++) {
                // if (events[i].data.fd == captures[0].getFd()) {
                    // do {
                        // index[0] = captures[0].readFrame();
                        // if (index[0] == -1)
                            // break;
                        // render.updateTexture(0, index[0]);
                        // captures[0].doneFrame(index[0]);
                    // }while (1);
                    // fCount++;
                // }
            // }
            int ret = poll(pfd, 4, -1);
            if (ret <= 0) {
                throw std::runtime_error("poll error or timeout");
            }
            fCount++;
            for (int i = 0; i < 4; i++) {
                if (pfd[i].revents & POLLIN) {
                    index[i] = captures[i].readFrame();
                    render.updateTexture(i, index[i]);
                    captures[i].doneFrame(index[i]);
                }
            }

            render.render(0);

            // for (size_t i = 0; i < captures.size(); i++) {

                // index[i] = captures[i].readFrame();

                // if (index[i] == -1) {
                    // continue;
                // }

                // fCount++;

                // if (i == 0)
                // render.updateTexture(i, index[i]);
                // captures[i].doneFrame(index[i]);
            // }
            // render.render(0);

            if (fCount != 0) {
                currentTime = glfwGetTime();
                frameCount++;
                double deltaT = currentTime - previousTime;
                if (deltaT >= 1.0) {
                    std::cout << frameCount / deltaT << std::endl;
                    frameCount = 0;
                    previousTime = currentTime;
                }
                fCount = 0;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    close(epoll_fd);

    return 0;
}
