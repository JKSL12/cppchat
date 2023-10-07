// ChatDlg.cpp: 구현 파일
//

#include "pch.h"
#include "Client.h"
#include "ChatDlg.h"
#include "afxdialogex.h"
#include "Net/Packet.h"

// ChatDlg 대화 상자

IMPLEMENT_DYNAMIC(CChatDlg, CDialogEx)

CChatDlg::CChatDlg(long account_seq, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHAT_DLG, pParent)
	, m_account_seq(account_seq)
	, client(this, account_seq)
	, m_scroll_chat_value(0)
{
}

CChatDlg::~CChatDlg()
{
}

void CChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ACCOUNT_SEQ, m_static_account_seq);
	DDX_Control(pDX, IDC_EDIT_CHAT, m_edit_chat);
	DDX_Control(pDX, IDC_STATIC_CHAT, m_static_chat);

	CString ac;
	ac.Format(_T("ID : %ld"), m_account_seq);
	m_static_account_seq.SetWindowTextW(ac);
	DDX_Control(pDX, IDC_SCROLLBAR_CHAT, m_scroll_chat);
	DDX_Scroll(pDX, IDC_SCROLLBAR_CHAT, m_scroll_chat_value);
}

BEGIN_MESSAGE_MAP(CChatDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CChatDlg::OnBtnSendClick)
END_MESSAGE_MAP()


// ChatDlg 메시지 처리기


void CChatDlg::OnBtnSendClick()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString chat;
	m_edit_chat.GetWindowTextW(chat);
	std::string str = CT2CA(chat);

	auto packet = new NET::CPacket(PROTOCOL::ID::GS_CHAT_REQ, sizeof(PROTOCOL::GS_CHAT_REQ));
	static_cast<PROTOCOL::GS_CHAT_REQ*>(packet->Body())->account_seq = m_account_seq;
	strncpy_s(static_cast<PROTOCOL::GS_CHAT_REQ*>(packet->Body())->szText, str.c_str(), str.size());
	client.GetSession()->_send(packet);

	m_edit_chat.SetSel(0, -1, TRUE);
	m_edit_chat.Clear();

	m_edit_chat.SetFocus();
}

void CChatDlg::RecvMsgEnterLeave(long account_seq, char* msg)
{
	CString chat;
	m_static_chat.GetWindowTextW(chat);

	CString append;
	append.Format(_T("====== [%Id] "), account_seq);

	CString newchat = chat + append + (LPSTR)msg + _T(" ======\n");

	m_static_chat.SetWindowTextW(newchat);
}

void CChatDlg::RecvMsg(long account_seq, char* msg)
{
	CString chat;
	m_static_chat.GetWindowTextW(chat);

	CString append;
	append.Format(_T("[%Id] : "), account_seq);

	CString newchat = chat + append + (LPSTR)msg + _T("\n");

	m_static_chat.SetWindowTextW(newchat);
}