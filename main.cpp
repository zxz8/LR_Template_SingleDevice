//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "main.h"
#include "ConfigCom.h"
#include <algorithm>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Tmainform *mainform;
//---------------------------------------------------------------------------
__fastcall Tmainform::Tmainform(TComponent* Owner)
	: TForm(Owner)
{
	try{
		std::vector<TMyIp> VC_MyIpPool;
		if(GetMyIP(VC_MyIpPool) == 0)
			{throw Exception(L"无可用网络连接");}
		this->m_ActiveIP = VC_MyIpPool.front();
		//如果通过配置服务软件打开本软件
		if(ParamCount() != 0){
			this->m_Params.AppIP = ParamStr(1);                    //本机IP
			this->m_Params.DeviceIP = ParamStr(2);                 //设备IP
			this->m_Params.ServerAppHwndName = ParamStr(3);        //服务窗体名
			const auto ipit = std::find_if(VC_MyIpPool.begin(),VC_MyIpPool.end(),
						   [&](const TMyIp &myip){if(myip.IpAddr == this->m_Params.AppIP)
													{return true;}
												  else
													{return false;}});
			this->m_ActiveIP = ipit == VC_MyIpPool.end()? VC_MyIpPool.front():*ipit;
			this->m_DeviceInfo.IP = this->m_Params.DeviceIP;
		}
		//独立打开
		else{
			this->m_ActiveIP = VC_MyIpPool.front();
		}
		SetBindings();
		DataToUI(TUIType::stab);
	}
	catch(Exception &err){
		ShowMessage(L"初始化异常" + err.Message);
	}
}
//---------------------------------------------------------------------------
//UDP设定
void __fastcall Tmainform::SetBindings(void)
{
	UDPSvr->Active = false;
	UDPSvr->Bindings->Clear();
	UDPSvr->Bindings->Add();
	UDPSvr->Bindings->Items[0]->IP = this->m_ActiveIP.IpAddr ;
	//UDPSvr->Bindings->Items[0]->Port = 2305;
	UDPSvr->Active = true;
}
//---------------------------------------------------------------------------
//菜单|通讯配置
void __fastcall Tmainform::N1Click(TObject *Sender)
{
	Tconfigform* pconfigform = new Tconfigform(this,this->m_ActiveIP,this->m_DeviceInfo);
	int modalresult = pconfigform->ShowModal();
	if(modalresult == mrOk){
		SetBindings();
		DataToUI(TUIType::stab);
		ShowMessage(L"配置成功");
	}
	delete pconfigform;
}
//---------------------------------------------------------------------------
void __fastcall Tmainform::DataToUI(TUIType UIType)
{
	switch (UIType) {
		//状态栏
		case TUIType::stab:{
			if(UDPSvr->Bindings->Count){
				StaB->Panels->Items[0]->Text = L"本机IP:" + UDPSvr->Bindings->Items[0]->IP;
				StaB->Panels->Items[1]->Text = L"本机端口:" + IntToStr(UDPSvr->Bindings->Items[0]->Port);
			}
			else{
				StaB->Panels->Items[0]->Text = L"本机IP:--";
                StaB->Panels->Items[1]->Text = L"本机端口:--";
			}
			break;
		}
	}
}
//---------------------------------------------------------------------------

