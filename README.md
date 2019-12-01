# DirectXVideoScreen

* D3D9RenderShaderYUV : ok
* D3D9VideoProcessor : ok
* D3D11ShaderNV12 : in progress
* D3D11VideoProcessor : ok
* DirectXVideoScreen : not started
* SimpleDesktopDuplication : ok


## D3D9RenderShaderYUV

A DirectX9 program to convert NV12/YV12 to RGBA with shader. I use a technique that I called "Shift for YUV texture".
This technique uses one or two Texture1D, so we can use the shader directly on the NV12/YV12 texture.

![ShowRGB](./DirectXVideoScreen/Media/ShowRGB.jpg)

![YUVShader](./DirectXVideoScreen/Media/YUVShader.jpg)
