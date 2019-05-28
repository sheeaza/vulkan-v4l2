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

            pfd[i].fd = captures[i].getFd();
            pfd[i].events = POLLIN;
        }

        int frameCount = 0;
        double previousTime = glfwGetTime();
        double currentTime;
        int fCount = 0;

        int index[4] = {0, 0, 0, 0};

        while (keepRunning) {
            glfwPollEvents();

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
