#include <iostream>
#include <fstream>
#include <Windows.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Foundation.h>
#include <string>
#include <chrono>
#include <windows.h>

using namespace std;
using namespace winrt;
using namespace Windows::Media::Control;
using namespace Windows::Foundation;
using namespace std::chrono;
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

int main() {
    init_apartment();
    auto smtcsm = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
    auto currentSession = smtcsm.GetCurrentSession();
    std::ofstream file("C:\\1.txt", std::ios::out | std::ios::trunc); // ÿ��ѭ��ʱ�����ļ����������
    if (!currentSession) {
        cerr << "No active media session found." << endl;
        return -1;
    }
    if (!file.is_open()) {
        cerr << "Unable to open file." << endl;
        return -1;
    }

    auto mediaPlaybackInfo = currentSession.GetTimelineProperties();
    auto startTime = std::chrono::steady_clock::now(); // ��¼��������ʱ����Ϊ��ʼʱ���
    auto lastPosition = mediaPlaybackInfo.Position(); // ��¼��һ�εĲ���λ��
    std::wstring lasttitleWStr = currentSession.TryGetMediaPropertiesAsync().get().Title().c_str();
    bool isPlaying = false; // ��־λ����ʾ��ǰý���Ƿ����ڲ���

    while (true) {
        auto smtcsm = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
        auto currentSession = smtcsm.GetCurrentSession();
        auto mediaPlaybackInfo = currentSession.GetTimelineProperties();
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
        std::wstring albumArtistWStr = currentSession.TryGetMediaPropertiesAsync().get().AlbumArtist().c_str();
        if (mediaPlaybackInfo) {
            auto position = mediaPlaybackInfo.Position();
            auto duration = mediaPlaybackInfo.MaxSeekTime();
            auto positionSeconds = std::chrono::duration_cast<std::chrono::seconds>(position).count();
            auto durationSeconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
            std::wstring titleWStr = currentSession.TryGetMediaPropertiesAsync().get().Title().c_str();
            std::wstring albumArtistWStr = currentSession.TryGetMediaPropertiesAsync().get().AlbumArtist().c_str();
            // ��鲥��λ���Ƿ����仯
            if (position != lastPosition || titleWStr != lasttitleWStr) {
                startTime = std::chrono::steady_clock::now(); // ���ü�ʱ��
                lastPosition = position; // ������һ�εĲ���λ��
                lasttitleWStr = titleWStr;
            }

            // ��鲥��״̬
            auto info = currentSession.GetPlaybackInfo();
            if (info.PlaybackStatus() ==
                GlobalSystemMediaTransportControlsSessionPlaybackStatus::Paused) {
                isPlaying = false;
                startTime = std::chrono::steady_clock::now(); // ���ü�ʱ��
            }
            else {
                isPlaying = true;
            }

            if (isPlaying) {
                std::ofstream file("C:\\1.txt", std::ios::out | std::ios::trunc); // ÿ��ѭ��ʱ�����ļ����������
                int albumArtistUtf8Length = WideCharToMultiByte(CP_UTF8, 0, albumArtistWStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
                int titleUtf8Length = WideCharToMultiByte(CP_UTF8, 0, titleWStr.c_str(), -1, nullptr, 0, nullptr, nullptr);

                std::string albumArtistUtf8(albumArtistUtf8Length, '\0');
                std::string titleUtf8(titleUtf8Length, '\0');

                WideCharToMultiByte(CP_UTF8, 0, albumArtistWStr.c_str(), -1, &albumArtistUtf8[0], albumArtistUtf8Length, nullptr, nullptr);
                WideCharToMultiByte(CP_UTF8, 0, titleWStr.c_str(), -1, &titleUtf8[0], titleUtf8Length, nullptr, nullptr);

                // �Ƴ�����Ŀ��ַ�
                albumArtistUtf8.pop_back();
                titleUtf8.pop_back();

                file << "Artist: " << albumArtistUtf8 << ", Title: " << titleUtf8 << ", Time: " << positionSeconds + elapsedTime << "/" << durationSeconds << " seconds" << std::endl;
            }

            file.close(); // ������ر��ļ�����ȷ��ÿ��ѭ���������´�
        }
        else {
            cerr << "Unable to retrieve media playback information." << endl;
        }

        Sleep(700);
    }

    // �ر��ļ�
    return 0;
}
