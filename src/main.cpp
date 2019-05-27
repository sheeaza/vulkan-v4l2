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

            struct epoll_event event = {};
            event.data.u32 = i;
            event.events = EPOLLIN; // do not use edge trigger
            int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, captures[i].getFd(),
                    &event);
            if (ret == -1) {
                throw std::runtime_error("EPOLL_CTL_ADD error");
            }
        }

        int frameCount = 0;
        double previousTime = glfwGetTime();
        double currentTime;
        int fCount = 0;

        int index[4] = {0, 0, 0, 0};

        struct epoll_event events[4];
        int nfd;

        while (keepRunning) {
            glfwPollEvents();

            nfd = epoll_wait(epoll_fd, events, 10, -1);
            if (nfd == -1)
                throw std::runtime_error("epoll_wait error, erron: " + std::to_string(errno));

            for (int i = 0; i < nfd; i++) {
                int data = events[i].data.u32;
                index[data] = captures[data].readFrame();
                if (index[data] == -1)
                    continue;
                render.updateTexture(data, index[data]);
                captures[data].doneFrame(index[data]);
                fCount++;
            }

            render.render(0);

            if (fCount != 0) {
                currentTime = glfwGetTime();
                frameCount++;
                double deltaT = currentTime - previousTime;
                if (deltaT >= 1.0) {
                    std::cout << "frame: " << frameCount / deltaT << std::endl;
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
