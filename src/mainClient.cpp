#include <iostream>
#include <fstream>
#include <string>
#include "networking/networking.hpp"
#include "login/login.hpp"
#include <windows.h>
#include <sodium.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_opengl3.h>
#include <stdio.h>
#include <SDL3/SDL.h>
#include <GL/gl.h>

std::string decrypt(std::string message, std::string clientPublicKey, std::string clientPrivateKey)
{
	char type = message[0];
	std::string msg = message.substr(1);
	size_t ciphertextLen = msg.size() - crypto_box_SEALBYTES;
	if (msg.size() < crypto_box_SEALBYTES)
	{
		std::cout << "message of size " << msg.size() << " shorter than crypto_box_SEALBYTES";
		return "";
	}
	std::vector<char> decrypted;
	decrypted.resize(ciphertextLen);
	if (crypto_box_seal_open((unsigned char*)decrypted.data(), (unsigned char*)msg.c_str(), msg.size(), (unsigned char*)clientPublicKey.c_str(), (unsigned char*)clientPrivateKey.c_str()) != 0)
	{
		std::cout << "crypto_box_seal_open() failed to decrypt ciphertext" << std::endl;
		return "";
	}
	decrypted.push_back(0);
	std::string decryptedMessage = type + std::string(decrypted.data());
	return decryptedMessage;
}

std::string encrypt(std::string message, std::string serverPublicKey)
{
	std::string msg = message.substr(1);
	size_t ciphertextLen = crypto_box_SEALBYTES + msg.size();
	std::vector<char> ciphertext;
	ciphertext.resize(ciphertextLen, 0);
	crypto_box_seal((unsigned char*)ciphertext.data(), (unsigned char*)msg.c_str(), msg.size(), (unsigned char*)serverPublicKey.c_str());
	ciphertext.push_back(0);
	std::string reply = message[0] + std::string(ciphertext.data());
	return reply;
}

int main() {
    // Setup SDL
// [If using SDL_MAIN_USE_CALLBACKS: all code below until the main loop starts would likely be your SDL_AppInit() function]
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        printf("Error: SDL_Init(): %s\n", SDL_GetError());
        return 1;
    }

    // Decide GL+GLSL versions

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    SDL_Window* window = SDL_CreateWindow("FlockChatter", (int)(1280 * main_scale), (int)(800 * main_scale), window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return 1;
    }
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr)
    {
        printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If fonts are not explicitly loaded, Dear ImGui will select an embedded font: either AddFontDefaultVector() or AddFontDefaultBitmap().
    //   This selection is based on (style.FontSizeBase * style.FontScaleMain * style.FontScaleDpi) reaching a small threshold.
    // - You can load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - If a file cannot be loaded, AddFont functions will return a nullptr. Please handle those errors in your code (e.g. use an assertion, display an error and quit).
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use FreeType for higher quality font rendering.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    //style.FontSizeBase = 20.0f;
    //io.Fonts->AddFontDefaultVector();
    //io.Fonts->AddFontDefaultBitmap();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
    //IM_ASSERT(font != nullptr);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    std::vector<char> connectAddress;
    connectAddress.resize(100);

    login::initSodium();

    Networking::initWinSock();

    bool connecting = false;
    bool connected = false;
    bool loggedIn = false;

    std::string clientPrivateKey, clientPublicKey, serverPublicKey, serverAddress, username, password;
    clientPublicKey.resize(crypto_box_PUBLICKEYBYTES, 0);
    clientPrivateKey.resize(crypto_box_SECRETKEYBYTES, 0);
    crypto_box_keypair((unsigned char*)clientPublicKey.c_str(), (unsigned char*)clientPrivateKey.c_str());
    
    Networking::ClientSocket socket(serverAddress, "8000");
    // Main loop
    bool done = false;

    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        // [If using SDL_MAIN_USE_CALLBACKS: call ImGui_ImplSDL3_ProcessEvent() from your SDL_AppEvent() function]
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppIterate() function]
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Connect To Server");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Enter server address");
        ImGui::InputText("", connectAddress.data(), connectAddress.size());
        if (ImGui::Button("Connect"))
        {
            
        }
        ImGui::End();
        

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }


    // Cleanup
    // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppQuit() function]
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

int oldmain() {
	login::initSodium();

	Networking::initWinSock();
	

	bool connected = false;
	bool loggedIn = false;

	std::string clientPrivateKey, clientPublicKey, serverPublicKey, serverAddress, username, password;
	clientPublicKey.resize(crypto_box_PUBLICKEYBYTES, 0);
	clientPrivateKey.resize(crypto_box_SECRETKEYBYTES, 0);
	crypto_box_keypair((unsigned char*)clientPublicKey.c_str(), (unsigned char*)clientPrivateKey.c_str());

	std::cout << "server address: ";
	std::cin >> serverAddress;

	//std::cout << "username: ";
	//std::cin >> username;

	//std::cout << "password: ";
	//std::cin >> password;

	std::cout << "connecting to server..." << std::endl;

	Networking::ClientSocket socket(serverAddress, "8000");
	//Sleep(1000);

	std::cout << "Exchanging public keys..." << std::endl;

	socket.send((char)Networking::MessageTypes::ExchangePublicKey + clientPublicKey);

	std::string message = socket.recv();

	while (message.size() == 0)
	{
		//std::cout << "waiting..." << std::endl;
		message = socket.recv();
		//Sleep(1000);
	}

	std::cout << message << std::endl;

	message = decrypt(message, clientPublicKey, clientPrivateKey);

	if(message.size() > 2)
		serverPublicKey = message.substr(1);

	std::cout << "serverPublicKey.size(): " << serverPublicKey.size() << std::endl;

	std::cout << "Getting server name..." << std::endl;

	socket.send(std::string(1, Networking::MessageTypes::GetServerName));

	message = "";
	while (message.size() == 0) {
		//std::cout << "waiting..." << std::endl;
		message = socket.recv();
		//Sleep(1000);
	}

	message = decrypt(message, clientPublicKey, clientPrivateKey);

	if(message.size() > 2)
		std::cout << "server name: " << message.substr(1) << std::endl;

	Networking::winSockCleanup();

	Sleep(10000);

	return 0;
}
