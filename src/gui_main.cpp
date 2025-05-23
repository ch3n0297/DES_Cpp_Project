// src/gui_main.cpp
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "des.h"  // 使用新的頭文件
#include <GLFW/glfw3.h>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <string>
#include "tinyfiledialogs.h"   // Native file‑picker dialogs

// Helper：將整數 key/iv 填入 bool[64]
static void initKeyIV(unsigned int keyInt, bool key64[64], bool iv64[64]) {
    memset(key64, false, 64);
    memset(iv64,  false, 64);
    for(int i=0;i<32;i++) key64[i] = ((keyInt>>i)&1)!=0;
}

int main() {
    // 1) 初始化 GLFW + OpenGL context
    if(!glfwInit()) return -1;

#ifdef __APPLE__
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on macOS
#else
    const char* glsl_version = "#version 330";
#endif

    // 在建立視窗前先設定可縮放 (允許外部視窗縮放)
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);   // 允許外部視窗縮放

    // 假設以 640x384 作為近似 80x24 的視窗大小
    GLFWwindow* window = glfwCreateWindow(
        640, 384,             // 寬度640，高度384
        "DES GUI", NULL, NULL
    );
    if(!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // 2) 初始化 ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // 3) GUI 狀態
    int  mode   = 0;            // 0 = not selected
    int  op     = 0;            // 0 = not selected
    char inPath[256]  = "";
    char outPath[256] = "";
    unsigned int keyInt = 3000;
    bool key64[64], iv64[64];
    static std::string statusMsg = "";
    static bool statusError = false;   // true = error, false = success/info

    // 4) 主迴圈
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --- 建立 GUI ---
        // 依照目前顯示區域大小，第一次顯示時就全滿外部視窗
        ImVec2 display_sz = ImGui::GetIO().DisplaySize;
        ImGui::SetNextWindowSize(display_sz, ImGuiCond_FirstUseEver);
        ImGui::Begin("Data Encrypt Standard");

        ImGui::InputInt("Key (integer)", (int*)&keyInt);

        // --- Input file picker ---
        ImGui::InputText("Input File", inPath, 256,
                         ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        if (ImGui::Button("Browse##in")) {
            const char* fp = tinyfd_openFileDialog(
                "Open Input File", "", 0, nullptr, nullptr, 0);
            if (fp) { std::strncpy(inPath, fp, 255); inPath[255] = '\0'; }
        }

        // --- Output file picker ---
        ImGui::InputText("Output File", outPath, 256,
                         ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        if (ImGui::Button("Browse##out")) {
            const char* fp = tinyfd_saveFileDialog(
                "Select Output File", "", 0, nullptr, nullptr);
            if (fp) { std::strncpy(outPath, fp, 255); outPath[255] = '\0'; }
        }

        const char* modes[] = {"-- Select Mode --","ECB","CFB","CTR"};
        ImGui::Combo("Mode", &mode, modes, IM_ARRAYSIZE(modes));
        const char* ops[]   = {"-- Select Action --","Encrypt","Decrypt"};
        ImGui::Combo("Action", &op,   ops,   IM_ARRAYSIZE(ops));

        if(ImGui::Button("Execute")) {
            statusMsg.clear();
            statusError = false;

            if(inPath[0]=='\0')
            { statusMsg = "Please choose an *input* file."; statusError = true; }
            else if(outPath[0]=='\0')
            { statusMsg = "Please choose an *output* file."; statusError = true; }
            else if(mode == 0)
            { statusMsg = "Please select encryption *Mode*."; statusError = true; }
            else if(op == 0)
            { statusMsg = "Please select *Action* (Encrypt/Decrypt)."; statusError = true; }
            else {
                initKeyIV(keyInt, key64, iv64);
                FILE* fin  = fopen(inPath, "rb");
                FILE* fout = fopen(outPath,"wb");
                if(!fin||!fout) {
                    statusMsg = "Failed to open files.";
                    statusError = true;
                } else {
                    switch(mode) {
                        case 1: op==1 ? desECBEncrypt(fin,fout,key64) : desECBDecrypt(fin,fout,key64); break;
                        case 2: op==1 ? desCFBEncrypt(fin,fout,iv64,key64) : desCFBDecrypt(fin,fout,iv64,key64); break;
                        case 3: desCTREncryptDecrypt(fin,fout,iv64,key64); break;
                    }
                    fclose(fin); fclose(fout);
                    statusMsg = "Operation completed.";
                }
            }
        }

        if(!statusMsg.empty()) {
            ImVec4 col = statusError ? ImVec4(1.0f,0.25f,0.25f,1.0f) : ImVec4(0.25f,1.0f,0.25f,1.0f);
            ImGui::Spacing();
            ImGui::TextColored(col, "%s", statusMsg.c_str());
        }

        ImGui::End();
        // -------------

        ImGui::Render();
        int w,h; glfwGetFramebufferSize(window,&w,&h);
        glViewport(0,0,w,h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}