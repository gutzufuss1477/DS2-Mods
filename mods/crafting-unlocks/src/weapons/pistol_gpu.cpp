#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <d3d12.h>
#include "../../build/pistol_pixels.inc"
#include "../../build/shotgun_icon_pixels.inc"
#include "../../build/bigbore_icon_pixels.inc"

namespace {
struct Upload {
 ID3D12Resource *texture=nullptr,*staging=nullptr;
 ID3D12DescriptorHeap* heap=nullptr;
 ID3D12CommandQueue* queue=nullptr;
 ID3D12CommandAllocator* allocator=nullptr;
 ID3D12GraphicsCommandList* list=nullptr;
 ID3D12Fence* fence=nullptr;
 bool submitted=false,ready=false; HRESULT error=S_OK;
};
Upload upload,iconUploads[2];
struct Pixels {UINT width,height,mips,blockBytes;DXGI_FORMAT format;const unsigned char*data;SIZE_T size;};
constexpr Pixels bodyPixels={1024,1024,11,8,DXGI_FORMAT_BC1_UNORM_SRGB,PistolPixels,sizeof(PistolPixels)};
constexpr Pixels iconPixels[]={
 {512,320,1,16,DXGI_FORMAT_BC7_UNORM_SRGB,ShotgunIconPixels,sizeof(ShotgunIconPixels)},
 {512,320,1,16,DXGI_FORMAT_BC7_UNORM_SRGB,BigboreIconPixels,sizeof(BigboreIconPixels)}
};
template<class T>void drop(T*&p){if(p){p->Release();p=nullptr;}}
void scratch(Upload&u){drop(u.list);drop(u.allocator);drop(u.staging);drop(u.fence);drop(u.queue);}
HRESULT start(ID3D12Device*d,Upload&u,const Pixels&pixels){
 HRESULT hr;
 D3D12_RESOURCE_DESC tex={};tex.Dimension=D3D12_RESOURCE_DIMENSION_TEXTURE2D;
 tex.Width=pixels.width;tex.Height=pixels.height;tex.DepthOrArraySize=1;tex.MipLevels=(UINT16)pixels.mips;
 tex.Format=pixels.format;tex.SampleDesc.Count=1;
 D3D12_HEAP_PROPERTIES props={};props.Type=D3D12_HEAP_TYPE_DEFAULT;props.CreationNodeMask=props.VisibleNodeMask=1;
 hr=d->CreateCommittedResource(&props,D3D12_HEAP_FLAG_NONE,&tex,D3D12_RESOURCE_STATE_COPY_DEST,nullptr,IID_PPV_ARGS(&u.texture));if(FAILED(hr))return hr;
 D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprints[11]={};UINT rows[11]={};UINT64 rowBytes[11]={},total=0;
 d->GetCopyableFootprints(&tex,0,pixels.mips,0,footprints,rows,rowBytes,&total);
 if(!total||total>2*1024*1024)return E_INVALIDARG;
 D3D12_RESOURCE_DESC buf={};buf.Dimension=D3D12_RESOURCE_DIMENSION_BUFFER;buf.Width=total;buf.Height=1;
 buf.DepthOrArraySize=buf.MipLevels=1;buf.SampleDesc.Count=1;buf.Layout=D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
 props.Type=D3D12_HEAP_TYPE_UPLOAD;
 hr=d->CreateCommittedResource(&props,D3D12_HEAP_FLAG_NONE,&buf,D3D12_RESOURCE_STATE_GENERIC_READ,nullptr,IID_PPV_ARGS(&u.staging));if(FAILED(hr))return hr;
 void*map=nullptr;D3D12_RANGE range={0,0};hr=u.staging->Map(0,&range,&map);if(FAILED(hr))return hr;
 SIZE_T offset=0;
 for(UINT i=0;i<pixels.mips;++i){
  UINT blocksX=((pixels.width>>i)+3)/4,blocksY=((pixels.height>>i)+3)/4;
  if(!blocksX)blocksX=1;if(!blocksY)blocksY=1;
  if(rows[i]!=blocksY||rowBytes[i]!=UINT64(blocksX)*pixels.blockBytes||offset+rows[i]*rowBytes[i]>pixels.size){
   u.staging->Unmap(0,nullptr);return E_INVALIDARG;
  }
  for(UINT r=0;r<rows[i];++r){
   auto*dst=(unsigned char*)map+footprints[i].Offset+SIZE_T(r)*footprints[i].Footprint.RowPitch;
   for(UINT64 b=0;b<rowBytes[i];++b)dst[b]=pixels.data[offset++];
  }
 }
 u.staging->Unmap(0,nullptr);if(offset!=pixels.size)return E_INVALIDARG;
 D3D12_DESCRIPTOR_HEAP_DESC hd={};hd.Type=D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;hd.NumDescriptors=1;
 hr=d->CreateDescriptorHeap(&hd,IID_PPV_ARGS(&u.heap));if(FAILED(hr))return hr;
 D3D12_SHADER_RESOURCE_VIEW_DESC srv={};srv.Format=tex.Format;srv.ViewDimension=D3D12_SRV_DIMENSION_TEXTURE2D;
 // Red, blue, blue, alpha: ochre panels become red; neutral metal stays neutral.
 srv.Shader4ComponentMapping=D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(0,2,2,3);srv.Texture2D.MipLevels=pixels.mips;
 d->CreateShaderResourceView(u.texture,&srv,u.heap->GetCPUDescriptorHandleForHeapStart());
 D3D12_COMMAND_QUEUE_DESC qd={};qd.Type=D3D12_COMMAND_LIST_TYPE_DIRECT;
 hr=d->CreateCommandQueue(&qd,IID_PPV_ARGS(&u.queue));if(FAILED(hr))return hr;
 hr=d->CreateCommandAllocator(qd.Type,IID_PPV_ARGS(&u.allocator));if(FAILED(hr))return hr;
 hr=d->CreateCommandList(0,qd.Type,u.allocator,nullptr,IID_PPV_ARGS(&u.list));if(FAILED(hr))return hr;
 hr=d->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&u.fence));if(FAILED(hr))return hr;
 for(UINT i=0;i<pixels.mips;++i){
  D3D12_TEXTURE_COPY_LOCATION dst={};dst.pResource=u.texture;dst.Type=D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;dst.SubresourceIndex=i;
  D3D12_TEXTURE_COPY_LOCATION src={};src.pResource=u.staging;src.Type=D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;src.PlacedFootprint=footprints[i];
  u.list->CopyTextureRegion(&dst,0,0,0,&src,nullptr);
 }
 D3D12_RESOURCE_BARRIER barrier={};barrier.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
 barrier.Transition.pResource=u.texture;barrier.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
 barrier.Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_DEST;
 barrier.Transition.StateAfter=D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE|D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
 u.list->ResourceBarrier(1,&barrier);hr=u.list->Close();if(FAILED(hr))return hr;
 ID3D12CommandList*lists[]={u.list};u.queue->ExecuteCommandLists(1,lists);u.submitted=true;
 return u.queue->Signal(u.fence,1);
}
// One persistent independent texture. Never retain a placed/streamed donor GPU
// allocation: the game's streaming allocator could recycle its backing memory.
int poll(Upload&u,const Pixels&pixels,void*device,UINT64*resource,UINT64*descriptor){
 if(!device||!resource||!descriptor)return -1;
 if(FAILED(u.error))return -1;
 if(!u.texture){u.error=start((ID3D12Device*)device,u,pixels);if(FAILED(u.error)){
  // A failed signal may still leave submitted GPU work. Retain its inputs.
  if(!u.submitted){scratch(u);drop(u.texture);drop(u.heap);}return -1;
 }}
 if(!u.ready){auto completed=u.fence->GetCompletedValue();
  if(completed==UINT64(-1)){u.error=DXGI_ERROR_DEVICE_REMOVED;return -1;}
  if(completed<1)return 0;u.ready=true;scratch(u);
 }
 *resource=(UINT64)u.texture;*descriptor=u.heap->GetCPUDescriptorHandleForHeapStart().ptr;return 1;
}
}
extern "C" __declspec(dllexport) int PistolGpuPoll(void*d,UINT64*r,UINT64*h){return poll(upload,bodyPixels,d,r,h);}
extern "C" __declspec(dllexport) int PrivateIconGpuPoll(unsigned int slot,void*d,UINT64*r,UINT64*h){return slot<2?poll(iconUploads[slot],iconPixels[slot],d,r,h):-1;}
extern "C" __declspec(dllexport) unsigned long PistolGpuError(){return (unsigned long)upload.error;}
extern "C" __declspec(dllexport) unsigned long PrivateIconGpuError(unsigned int slot){return slot<2?(unsigned long)iconUploads[slot].error:(unsigned long)E_INVALIDARG;}
