// LoginDlg.cpp: 구현 파일
//

#include "pch.h"
//#include "framework.h"
#include "Client.h"
#include "LoginDlg.h"
#include "afxdialogex.h"
#include "http_client.h"
#include "ChatDlg.h"

// LoginDlg 대화 상자

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

CLoginDlg::CLoginDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGIN_DLG, pParent)
{
	url_loginServer = "http://127.0.0.1:30000";
}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ID, m_edit_id);
	DDX_Control(pDX, IDC_PWD, m_edit_pwd);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLoginDlg::OnOkBtnClick)
	ON_BN_CLICKED(IDCANCEL, &CLoginDlg::OnBtnCancelClick)
END_MESSAGE_MAP()


// LoginDlg 메시지 처리기


void CLoginDlg::OnOkBtnClick()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnOK();
	CString cid;
	m_edit_id.GetWindowTextW(cid);
	std::string id = CT2CA(cid);

	CString cpwd;
	m_edit_pwd.GetWindowTextW(cpwd);
	std::string pwd = CT2CA(cpwd);

	if (id.empty() || pwd.empty())
	{
		MessageBox(_T("입력"), _T("err"), MB_OK);
		return;
	}

	std::ostringstream oss;
	oss << "id=" << id << "&pwd=" << pwd;
	std::string fullURL = url_loginServer + "/LoginProcess";

	http_client::Instance()->Request_Http_Post(0, fullURL, oss.str(), [this](std::string str)
		{
			rapidjson::Document doc;
			doc.Parse(str.c_str());

			if (doc.HasParseError()) {
				return;
			}

			bool result = doc["result"].GetBool();
			if (result == false) return;

			long account_seq = doc["account_seq"].GetInt64();
			std::string ip = doc["ip"].GetString();
			int port = atoi(doc["port"].GetString());

			CChatDlg chatDlg(account_seq, this);
			result = chatDlg.client.Initialize(std::wstring(ip.begin(), ip.end()).c_str(), port);

			if (result == false) return;
			chatDlg.client.Connect();

			chatDlg.client.Run();

			this->ShowWindow(SW_HIDE);
			
			INT_PTR nResponse = chatDlg.DoModal();
			if (nResponse == IDOK)
			{
				// TODO: 여기에 [확인]을 클릭하여 대화 상자가 없어질 때 처리할
				//  코드를 배치합니다.
				CDialogEx::OnOK();
			}
			else if (nResponse == IDCANCEL)
			{
				// TODO: 여기에 [취소]를 클릭하여 대화 상자가 없어질 때 처리할
				//  코드를 배치합니다.
				CDialogEx::OnCancel();
			}
			else if (nResponse == -1)
			{
				TRACE(traceAppMsg, 0, "경고: 대화 상자를 만들지 못했으므로 애플리케이션이 예기치 않게 종료됩니다.\n");
				TRACE(traceAppMsg, 0, "경고: 대화 상자에서 MFC 컨트롤을 사용하는 경우 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS를 수행할 수 없습니다.\n");
			}
			

			//CDialogEx::OnCancel();
		});
}


void CLoginDlg::OnBtnCancelClick()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}
