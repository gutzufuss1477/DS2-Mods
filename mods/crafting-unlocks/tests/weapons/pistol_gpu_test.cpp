// Real D3D12 upload/readback, independent of DS2 and its running process.
#include "../../src/weapons/pistol_gpu.cpp"
extern "C" int _fltused=0;
extern "C" void* memset(void*d,int v,SIZE_T n){auto*p=(volatile unsigned char*)d;while(n--)*p++=(unsigned char)v;return d;}
extern "C" void* memcpy(void*d,const void*s,SIZE_T n){auto*a=(volatile unsigned char*)d;auto*b=(const unsigned char*)s;while(n--)*a++=*b++;return d;}
int validateGpu(int slot){
 const auto&pixels=slot>=0?iconPixels[slot]:bodyPixels;
 ID3D12Device*d=nullptr;HRESULT hr=D3D12CreateDevice(nullptr,D3D_FEATURE_LEVEL_11_0,IID_PPV_ARGS(&d));if(FAILED(hr))return 10;
 UINT64 resource=0,descriptor=0;int status=0;
 for(int i=0;i<500&&status==0;++i){status=slot>=0?PrivateIconGpuPoll((unsigned int)slot,d,&resource,&descriptor):PistolGpuPoll(d,&resource,&descriptor);Sleep(10);}
 if(status!=1||!resource||!descriptor)return 11;
 auto*t=(ID3D12Resource*)resource;auto desc=t->GetDesc();
 if(desc.Width!=pixels.width||desc.Height!=pixels.height||desc.MipLevels!=pixels.mips||desc.Format!=pixels.format)return 12;
 D3D12_PLACED_SUBRESOURCE_FOOTPRINT fp[11]={};UINT rows[11]={};UINT64 rb[11]={},total=0;
 d->GetCopyableFootprints(&desc,0,pixels.mips,0,fp,rows,rb,&total);
 ID3D12Resource*readback=nullptr;D3D12_HEAP_PROPERTIES hp={};hp.Type=D3D12_HEAP_TYPE_READBACK;hp.CreationNodeMask=hp.VisibleNodeMask=1;
 D3D12_RESOURCE_DESC b={};b.Dimension=D3D12_RESOURCE_DIMENSION_BUFFER;b.Width=total;b.Height=b.DepthOrArraySize=b.MipLevels=1;b.SampleDesc.Count=1;b.Layout=D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
 if(FAILED(d->CreateCommittedResource(&hp,D3D12_HEAP_FLAG_NONE,&b,D3D12_RESOURCE_STATE_COPY_DEST,nullptr,IID_PPV_ARGS(&readback))))return 13;
 ID3D12CommandQueue*q=nullptr;ID3D12CommandAllocator*a=nullptr;ID3D12GraphicsCommandList*l=nullptr;ID3D12Fence*f=nullptr;
 D3D12_COMMAND_QUEUE_DESC qd={};
 if(FAILED(d->CreateCommandQueue(&qd,IID_PPV_ARGS(&q)))||FAILED(d->CreateCommandAllocator(qd.Type,IID_PPV_ARGS(&a)))||
    FAILED(d->CreateCommandList(0,qd.Type,a,nullptr,IID_PPV_ARGS(&l)))||FAILED(d->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&f))))return 14;
 D3D12_RESOURCE_BARRIER barrier={};barrier.Type=D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;barrier.Transition.pResource=t;
 barrier.Transition.Subresource=D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
 barrier.Transition.StateBefore=D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE|D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
 barrier.Transition.StateAfter=D3D12_RESOURCE_STATE_COPY_SOURCE;l->ResourceBarrier(1,&barrier);
 for(UINT i=0;i<pixels.mips;++i){D3D12_TEXTURE_COPY_LOCATION dst={},src={};dst.pResource=readback;dst.Type=D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;dst.PlacedFootprint=fp[i];
  src.pResource=t;src.Type=D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;src.SubresourceIndex=i;l->CopyTextureRegion(&dst,0,0,0,&src,nullptr);}
 if(FAILED(l->Close()))return 15;ID3D12CommandList*lists[]={l};q->ExecuteCommandLists(1,lists);if(FAILED(q->Signal(f,1)))return 16;
 for(int i=0;i<500&&f->GetCompletedValue()==0;++i)Sleep(10);
 if(f->GetCompletedValue()!=1)return 17;
 void*data=nullptr;D3D12_RANGE range={0,(SIZE_T)total};if(FAILED(readback->Map(0,&range,&data)))return 18;
 SIZE_T off=0;bool equal=true;
 for(UINT i=0;i<pixels.mips;++i)for(UINT r=0;r<rows[i];++r)for(UINT64 col=0;col<rb[i];++col)
  if(((unsigned char*)data)[fp[i].Offset+UINT64(r)*fp[i].Footprint.RowPitch+col]!=pixels.data[off++])equal=false;
 readback->Unmap(0,nullptr);drop(readback);drop(l);drop(a);drop(f);drop(q);drop(d);
 return equal&&off==pixels.size?0:19;
}
extern "C" __declspec(dllexport) int ValidatePistolGpu(){return validateGpu(-1);}
extern "C" __declspec(dllexport) int ValidatePrivateIconGpu(unsigned int slot){return slot<2?validateGpu((int)slot):20;}
