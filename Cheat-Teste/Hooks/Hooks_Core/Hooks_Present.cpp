#include "../Hooks_Core/Hooks_Present.h"
#include <D3DX11.h>
#include <d3d11.h>
#include "../../Menu/Menu.h"
#include "../../ImGui/imgui_impl_dx11.h"
#include "../../ImGui/imgui_impl_win32.h"
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3d11.lib")
#include "../Minhook/MinHook.h"


//Aqui vamos definir nossas variaveis pra fazer a render e preparar o present para colocar o nosso conteudo l� dentro

//Um Typedef para o Present original, � para onde n�s vamos saltar no final da nossa hook
typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);

//Fazemos um Typedef para um CallBack da window do jogo, aqui dentro podemos fazer tratamentos na windows, como o LockCursor
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

//Aqui temos outra fun��o do IMGUi para tratar as windows, onde tem a mensagem, o wparam e lparam s�o mensagens adicionais
//mas o foco ta UINT msg.
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


//Uma estrutura que vai guardar o present original
Present oPresent;

//Essa variavel vai ficar guardada uma Handle para a Window do Menu e do jogo
HWND window = NULL;

//Essa variavel, � uma estrutura que vai guardar a Procedure original do jogo, onde vai ficar a Window do jogo.
WNDPROC oWndProc;

//Aqui fica o dispositivo grafico do DirectX11, nele fica salvo configura��es importantes em rela��o a forma que a render vai se comportar
ID3D11Device* pDevice = NULL;

//Aqui � uma variavel que representa o contexto do pDevice, ela � responsavel pra enviar solicita��es de renderiza��o, e controla como a render vai ser feita.
ID3D11DeviceContext* pContext = NULL;

//Aqui � o destino final da render do present, um pointer para a o frame atual.
ID3D11RenderTargetView* mainRenderTargetView;

//Uma "Hook" da Window do jogo, onde vai ser redirecionada pra ca, antes das a��es como cliques cheguem na janela do jogo.
LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
    if (Menu::IsVisible)
    {
        WM_RBUTTONDOWN;
        return 1;
    }
    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    if (!KieroHooks::Imgui_Init)
    {
        //a gente usa um if succeded pra chegar se conseguimos pegar o dispositivo do DirectX11, se conseguimos, podemos continuar
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
        {
            //Aqui a gente obtem um pointer para o contexto do present, como eu expliquei anteriormente no pContext
            //Essa fun��o ira pegar o pContext, e torna-la um pointer para o context do present do jogo.
            pDevice->GetImmediateContext(&pContext);

            //Aqui � o DXGI_SWAP_CHAIN_DESC, � uma simples estruturas que contem informa��es sobre os buffers de render, n�s usamos essas informa��es para alterar o comportamento das renders
            DXGI_SWAP_CHAIN_DESC sd;

            //Aqui a gente coloca justamente as informa��es dentro do DXGI_SWAP_CHAIN_DESC
            pSwapChain->GetDesc(&sd);

            //window = � basicamente nossa window do jogo, onde a render vai acontecer.
            window = sd.OutputWindow;

            //Aqui � onde fica uma especie de variavel onde fica guardada a render final, at� que a troca de buffers pela nova render aconteca.
            ID3D11Texture2D* pBackBuffer;

            //aqui a gente atribui justamente a render para esse pBackBuffer
            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

            //aqui � basicamente para que tenhamos controle sobre a render do nosso IMGUi, ESP, SNAPLINES e ESPBOX
            pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);

            //Limpamos o pBackBuffer
            pBackBuffer->Release();

            //Aqui a gente substitui a WndProc original pela nossa, ou seja, � basicamente uma hook.
            oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);

            //Inicializamos o ImGui
            KieroHooks::InitImgui();

            //Criamos/Inicializamos a estiliza�� do IMGUI, como fontes, cores, etc..
            Menu::Style();

            //E setamos essa variavel como true pois esse bloco de codigo s� precisa ser feito uma unica vez, a gente j� possui
            //O controle da render, que foi na linha: pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
            KieroHooks::Imgui_Init = true;

        }
        //Se n�o conseguirmos fazer a fun��o, apenas execute o Present original, isso evita crash do jogo
        else
            return oPresent(pSwapChain, SyncInterval, Flags);
    }

    //Render da logo do menu, na verdade s� estamos criando uma especie de buffer para a render do nosso logo.
    static bool fisr = true;
    if (fisr)
    {


        fisr = false;
    }

    //Ativar/Desativar Menu
    if (GetAsyncKeyState(VK_INSERT) & 1)
    {
        Menu::IsVisible = !Menu::IsVisible;

    }

    //Criamos uma nova frame para renderizar o IMGUi e o restante l� dentro
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    if (Menu::IsVisible)
    {
        Menu::Render();
    }


    ImGui::EndFrame();
    ImGui::Render();

    //Colocamos toda a newframe e renders que fizemos dentro da mainRenderTargetView, a gente obteve esse controle ali em cima
    //No codigo "pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);"
    pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);

    //Aqui a gente instrui ao imgui renderizar o conteudo dentro do nosso DirectX11
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    //Retornamos a fun��o original do Present, para renderizar o jogo
    return oPresent(pSwapChain, SyncInterval, Flags);
}



void KieroHooks::Init()
{

    bool should_load = false;
    do
    {

        if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
        {
            kiero::bind(8, (void**)&oPresent, hkPresent);
            should_load = true;
        }

    } while (should_load == false);

}

void KieroHooks::Shutdown()
{
    MH_DisableHook(MH_ALL_HOOKS);
}

void KieroHooks::InitImgui()
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX11_Init(pDevice, pContext);
    ImGui::GetIO().MouseDrawCursor = true;
}
