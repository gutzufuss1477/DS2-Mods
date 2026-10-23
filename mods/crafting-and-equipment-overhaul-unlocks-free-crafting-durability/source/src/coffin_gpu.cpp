// Resident GPU color texture for a SINGLE DS2 Coffinboard red material instance.
// The original 2048x2048 color, normal, ORM maps and all streamed resources
// remain unmodified.  This ASI owns its independent D3D12 texture and SRV.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <d3d12.h>
#include "../build/coffin_red_pixels.inc"
namespace {
constexpr unsigned kWidth=2048,kHeight=2048,kMips=12,kBlockBytes=8;
constexpr SIZE_T kBytes=sizeof(CoffinRedPixels);
struct Upload {
 ID3D12Resource*tex=nullptr,*stage=nullptr;
 ID3D12DescriptorHeap*heap=nullptr;
 ID3D12CommandQueue*queue=nullptr;
 ID3D12CommandAllocator*alloc=nullptr;
 ID3D12GraphicsCommandList*list=nullptr;
 ID3D12Fence*fence=nullptr;
 bool submitted=false,ready=false;
 HRESULT result=S_OK;
};
Upload u;
template<class T>void release(T*&p){if(p){p->Release();p=nullptr;}}
void cleanup(){
 release(u.list);release(u.alloc);release(u.stage);
 release(u.fence);release(u.queue);
}
HRESULT prepare(ID3D12Device*d){
 if(!d||kBytes!=2796216u)return E_INVALIDARG;
 HRESULT hr;
 D3D12_RESOURCE_DESC tex={};
 tex.Dimension=D3D12_RESOURCE_DIMENSION_TEXTURE2D;
 tex.Width=kWidth;tex.Height=kHeight;tex.DepthOrArraySize=1;
 tex.MipLevels=kMips;tex.Format=DXGI_FORMAT_BC1_UNORM_SRGB;
 tex.SampleDesc.Count=1;
 D3D12_HEAP_PROPERTIES heap={};heap.Type=D3D12_HEAP_TYPE_DEFAULT;
 heap.CreationNodeMask=heap.VisibleNodeMask=1;
 hr=d->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&tex,
      D3D12_RESOURCE_STATE_COPY_DEST,nullptr,IID_PPV_ARGS(&u.tex));
 if(FAILED(hr))return hr;
 D3D12_PLACED_SUBRESOURCE_FOOTPRINT layouts[kMips]={};
 UINT rows[kMips]={};UINT64 rowBytes[kMips]={},total=0;
 d->GetCopyableFootprints(&tex,0,kMips,0,layouts,rows,rowBytes,&total);
 if(!total||total>8*1024*1024ull)return E_INVALIDARG;
 D3D12_RESOURCE_DESC buf={};
 buf.Dimension=D3D12_RESOURCE_DIMENSION_BUFFER;
 buf.Width=total;buf.Height=1;buf.DepthOrArraySize=buf.MipLevels=1;
 buf.SampleDesc.Count=1;buf.Layout=D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
 heap.Type=D3D12_HEAP_TYPE_UPLOAD;
 hr=d->CreateCommittedResource(&heap,D3D12_HEAP_FLAG_NONE,&buf,
       D3D12_RESOURCE_STATE_GENERIC_READ,nullptr,IID_PPV_ARGS(&u.stage));
 if(FAILED(hr))return hr;
 unsigned char*mapped=nullptr;D3D12_RANGE range={0,0};
 hr=u.stage->Map(0,&range,(void**)&mapped);
 if(FAILED(hr))return hr;
 SIZE_T index=0;bool valid=true;
 for(UINT mip=0;mip<kMips;++mip){
  UINT w=kWidth>>mip,h=kHeight>>mip;
  if(!w)w=1;if(!h)h=1;
  const UINT bx=(w+3)/4,by=(h+3)/4;
  if(rows[mip]!=by||rowBytes[mip]!=UINT64(bx)*kBlockBytes ||
     index+SIZE_T(by)*bx*kBlockBytes>kBytes){valid=false;break;}
  for(UINT row=0;row<by;++row){
   auto*dst=mapped+layouts[mip].Offset+SIZE_T(row)*layouts[mip].Footprint.RowPitch;
   for(UINT col=0;col<bx*kBlockBytes;++col)dst[col]=CoffinRedPixels[index++];
  }
 }
 u.stage->Unmap(0,nullptr);
 if(!valid||index!=kBytes)return E_INVALIDARG;
 D3D12_DESCRIPTOR_HEAP_DESC hd={};
 hd.Type=D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;hd.NumDescriptors=1;
 hr=d->CreateDescriptorHeap(&hd,IID_PPV_ARGS(&u.heap));
 if(FAILED(hr))return hr;
 D3D12_SHADER_RESOURCE_VIEW_DESC srv={};
 srv.Format=tex.Format;srv.ViewDimension=D3D12_SRV_DIMENSION_TEXTURE2D;
 // No swizzle: the authored albedo is already deep red with untouched decals.
 srv.Shader4ComponentMapping=D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
 srv.Texture2D.MipLevels=kMips;
 d->CreateShaderResourceView(u.tex,&srv,u.heap->GetCPUDescriptorHandleForHeapStart());
 D3D12_COMMAND_QUEUE_DESC q={};q.Type=D3D12_COMMAND_LIST_TYPE_DIRECT;
 hr=d->CreateCommandQueue(&q,IID_PPV_ARGS(&u.queue));if(FAILED(hr))return hr;
 hr=d->CreateCommandAllocator(q.Type,IID_PPV_ARGS(&u.alloc));if(FAILED(hr))return hr;
 hr=d->CreateCommandList(0,q.Type,u.alloc,nullptr,IID_PPV_ARGS(&u.list));
 if(FAILED(hr))return hr;
 hr=d->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&u.fence));
 if(FAILED(hr))return hr;
 for(UINT i=0;i<kMips;++i){
  D3D12_TEXTURE_COPY_LOCATION dst={},src={};
  dst.pResource=u.tex;dst.Type=D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
  dst.SubresourceIndex=i;
  src.pResource=u.stage;src.Type=D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
  src.PlacedFootprint=layouts[i];
  u.list->CopyTextureRegion(&dst,0,0,0,&src,nullptr);
 }
 D3D12_RESOURCE_BARRIER barrier={};
 barrier.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
 barrier.Transition.pResource=u.tex;
 barrier.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
 barrier.Transition.StateBefore=D3D12_RESOURCE_STATE_COPY_DEST;
 barrier.Transition.StateAfter=D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE|
   D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
 u.list->ResourceBarrier(1,&barrier);
 hr=u.list->Close();if(FAILED(hr))return hr;
 ID3D12CommandList*lists[]={u.list};
 u.queue->ExecuteCommandLists(1,lists);u.submitted=true;
 return u.queue->Signal(u.fence,1);
}
}
extern "C" __declspec(dllexport)
int CoffinRedGpuPoll(void*device,UINT64*resource,UINT64*handle){
 if(!device||!resource||!handle||FAILED(u.result))return -1;
 if(!u.tex){
  u.result=prepare((ID3D12Device*)device);
  if(FAILED(u.result)){
   if(!u.submitted){cleanup();release(u.tex);release(u.heap);}
   return -1;
  }
 }
 if(!u.ready){
  const UINT64 finished=u.fence->GetCompletedValue();
  if(finished==UINT64(-1)){u.result=DXGI_ERROR_DEVICE_REMOVED;return -1;}
  if(finished<1)return 0;
  u.ready=true;cleanup();
 }
 *resource=(UINT64)u.tex;
 *handle=u.heap->GetCPUDescriptorHandleForHeapStart().ptr;
 return 1;
}
extern "C" __declspec(dllexport)
unsigned long CoffinRedGpuError(){return (unsigned long)u.result;}
