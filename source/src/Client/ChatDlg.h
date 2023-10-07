#pragma once

#include "GameClient.h"
// ChatDlg 대화 상자

class CChatDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CChatDlg)

public:
	CChatDlg(long account_seq, CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CChatDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHAT_DLG };
#endif

	void RecvMsgEnterLeave(long account_seq, char* msg);
	void RecvMsg(long account_seq, char* msg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CGameClient client;
	CStatic m_static_account_seq;
	CEdit m_edit_chat;
	CStatic m_static_chat;

private:
	long m_account_seq;
public:
	afx_msg void OnBtnSendClick();
	CScrollBar m_scroll_chat;
	int m_scroll_chat_value;
};
