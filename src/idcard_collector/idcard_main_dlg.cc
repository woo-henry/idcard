#include "pch.h"
#include "framework.h"
#include "idcard_config.h"
#include "idcard_app.h"
#include "idcard_about_dlg.h"
#include "idcard_main_dlg.h"
//////////////////////////////////////////////////////////////////////////
#define WM_REFRESH_IDCARD	WM_USER + 100

BEGIN_MESSAGE_MAP(IdCardMainDlg, CDialogEx)
	ON_WM_QUERYDRAGICON()
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_COLLECTION, &IdCardMainDlg::OnClickedCollection)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_REFRESH_IDCARD, OnRefreshIdCard)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////
IdCardMainDlg::IdCardMainDlg(CWnd* pParent)
	: CDialogEx(IDD_IDCARD_CONSOLE_DIALOG, pParent)
	, _collection_signal(nullptr)
	, _collection_thread(nullptr)
	, _collection_thread_handle(nullptr)
	, _exiting(false)
	, _timer_second(2)
{
	_icon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

IdCardMainDlg::~IdCardMainDlg()
{
	_exiting = true;

	if (_collection_signal)
	{
		SetEvent(_collection_signal);
	}

	_collection_thread->timed_join(_timer_second + 2000);

	if (_collection_signal)
	{
		CloseHandle(_collection_signal);
		_collection_signal = nullptr;
	}
}

void IdCardMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PEOPLE_NAME, _ctl_people_name);
	DDX_Control(pDX, IDC_PEOPLE_SEX, _ctl_people_sex);
	DDX_Control(pDX, IDC_PEOPLE_NATION, _ctl_people_nation);
	DDX_Control(pDX, IDC_PEOPLE_BIRTH_YEAR, _ctl_people_birth_year);
	DDX_Control(pDX, IDC_PEOPLE_BIRTH_MONTH, _ctl_people_birth_month);
	DDX_Control(pDX, IDC_PEOPLE_BIRTH_DAY, _ctl_people_birth_day);
	DDX_Control(pDX, IDC_PEOPLE_ADDRESS, _ctl_people_address);
	DDX_Control(pDX, IDC_PEOPLE_ID_CODE, _ctl_people_id_code);
	DDX_Control(pDX, IDC_SIGN_DEPARTMENT, _ctl_sign_department);
	DDX_Control(pDX, IDC_VALID_DATE, _ctl_valid_date);
	DDX_Control(pDX, IDC_STATUS, _ctl_status);
}

BOOL IdCardMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(_icon, TRUE);
	SetIcon(_icon, FALSE);

	_ctl_people_name.SetWindowText(L"");
	_ctl_people_sex.SetWindowText(L"");
	_ctl_people_nation.SetWindowText(L"");
	_ctl_people_birth_year.SetWindowText(L"");
	_ctl_people_birth_month.SetWindowText(L"");
	_ctl_people_birth_day.SetWindowText(L"");
	_ctl_people_id_code.SetWindowText(L"");
	_ctl_people_address.SetWindowText(L"");
	_ctl_sign_department.SetWindowText(L"");
	_ctl_valid_date.SetWindowText(L"");
	_ctl_status.SetWindowText(L"");
	GetDlgItem(IDC_COLLECTION)->EnableWindow(false);
	
	_timer_second = IdCardConfig::GetInstance()->GetTimerSecond() * 1000;
	bool test = IdCardConfig::GetInstance()->IsTest();
	_collection_signal = CreateEventW(nullptr, true, false, nullptr);
	if (_collection_signal == nullptr)
	{
		AfxMessageBox(IDS_COLLECTION_EVENT_INIT_FAILED);
		return FALSE;
	}

	if (test)
	{
		boost::thread test_thread(boost::bind(&IdCardMainDlg::DoTest, this));
	}
	
	_collection_thread.reset(new boost::thread(boost::bind(&IdCardMainDlg::DoCollecting, this)));

	return TRUE;
}

HCURSOR IdCardMainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(_icon);
}

void IdCardMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		IdCardAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void IdCardMainDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		CRect rect;
		GetClientRect(&rect);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, _icon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

void IdCardMainDlg::OnClose()
{
	CDialogEx::OnClose();
}



void IdCardMainDlg::OnClickedCollection()
{	
	CString msg, title;
	title.LoadString(IDS_IDCARD_TITLE);
	GetDlgItem(IDC_STATUS)->ShowWindow(SW_HIDE);

	const char* output = IdCardConfig::GetInstance()->GetOutputTarget();
	const char* encoding = IdCardConfig::GetInstance()->GetEncoding();
	const char* people_name = _idcard_data->GetPeopleName();
	const char* people_sex = _idcard_data->GetPeopleSex();
	const char* people_nation = _idcard_data->GetPeopleNation();
	const char* people_birthday = _idcard_data->GetPeopleBirthday();
	int people_birthday_length = lstrlenA(people_birthday);
	const char* people_address = _idcard_data->GetPeopleAddress();
	const char* people_id_code = _idcard_data->GetPeopleIdCode();
	const char* sign_department = _idcard_data->GetSignDepartment();
	const char* valid_start_date = _idcard_data->GetValidStartDate();
	int valid_start_date_length = lstrlenA(valid_start_date);
	const char* valid_end_date = _idcard_data->GetValidEndDate();
	int valid_end_date_length = lstrlenA(valid_end_date);
	const char* bitmap_data = _idcard_data->GetBitmapData();
	unsigned int bitmap_size = _idcard_data->GetBitmapSize();
	
	std::string fmt_people_birthday;
	std::string fmt_valid_start_date;
	std::string fmt_valid_end_date;
	FormatDate(fmt_people_birthday, people_birthday, people_birthday_length);
	FormatDate(fmt_valid_start_date, valid_start_date, valid_start_date_length);
	FormatDate(fmt_valid_end_date, valid_end_date, valid_end_date_length);

	int result = IdCardManager::GetInstance()->OutputNetwork(output, encoding,
		people_name, people_sex, people_nation, fmt_people_birthday.c_str(), people_address, people_id_code, 
		sign_department, fmt_valid_start_date.c_str(), fmt_valid_end_date.c_str(), bitmap_data, bitmap_size);
	if (result == IDCARD_ERROR_SUCCESS)
	{
		msg.LoadString(IDS_COLLECTION_SUCCESS);
		GetDlgItem(IDC_STATUS)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATUS)->SetWindowText(msg);
		MessageBox(msg, title);
	}
	else
	{
		msg.LoadString(IDS_COLLECTION_FAILD);
		GetDlgItem(IDC_STATUS)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATUS)->SetWindowText(msg);
		MessageBox(msg, title);
	}
}

afx_msg LRESULT IdCardMainDlg::OnRefreshIdCard(WPARAM wParam, LPARAM lParam)
{
	LRESULT result = IDCARD_ERROR_SUCCESS;
	IdCardData* data = nullptr;

	do 
	{
		data = (IdCardData*)wParam;
		if (data == nullptr)
		{
			result = IDCARD_ERROR_FAILURE;
			BaseLogWarn("illegal ID card collection data");
			break;
		}

#ifdef _UNICODE
		boost::filesystem::wpath people_id_code(data->GetPeopleIdCode());
#else
		boost::filesystem::path people_id_code(data->GetPeopleIdCode());
#endif

		CString id_code;
		_ctl_people_id_code.GetWindowText(id_code);
		if (lstrcmp(id_code, people_id_code.c_str()) == 0)
			break;

#ifdef _UNICODE
		boost::filesystem::wpath people_name(data->GetPeopleName());
		boost::filesystem::wpath people_sex(data->GetPeopleSex());
		boost::filesystem::wpath people_nation(data->GetPeopleNation());
		boost::filesystem::wpath people_birthday(data->GetPeopleBirthday());
		boost::filesystem::wpath people_address(data->GetPeopleAddress());
		boost::filesystem::wpath sign_department(data->GetSignDepartment());
		boost::filesystem::wpath valid_start_date(data->GetValidStartDate());
		boost::filesystem::wpath valid_end_date(data->GetValidEndDate());
#else
		
		boost::filesystem::path people_name(data->GetPeopleName());
		boost::filesystem::path people_sex(data->GetPeopleSex());
		boost::filesystem::path people_nation(data->GetPeopleNation());
		boost::filesystem::path people_birthday(data->GetPeopleBirthday());
		boost::filesystem::path people_address(data->GetPeopleAddress());
		boost::filesystem::path sign_department(data->GetSignDepartment());
		boost::filesystem::path valid_start_date(data->GetValidStartDate());
		boost::filesystem::path valid_end_date(data->GetValidEndDate());
#endif
		_ctl_people_name.SetWindowText(people_name.c_str());
		_ctl_people_sex.SetWindowText(people_sex.c_str());
		_ctl_people_nation.SetWindowText(people_nation.c_str());

		CString people_birthday_string(people_birthday.c_str());
		CString people_birth_year = people_birthday_string.Left(4);
		CString people_birth_month = people_birthday_string.Mid(4, 2);
		CString people_birth_day = people_birthday_string.Right(2);

		_ctl_people_birth_year.SetWindowText(people_birth_year);
		_ctl_people_birth_month.SetWindowText(people_birth_month);
		_ctl_people_birth_day.SetWindowText(people_birth_day);

		_ctl_people_address.SetWindowText(people_address.c_str());
		_ctl_people_id_code.SetWindowText(people_id_code.c_str());
		_ctl_sign_department.SetWindowText(sign_department.c_str());

		CString people_valid_start_date(valid_start_date.c_str());
		CString people_valid_end_date(valid_end_date.c_str());
		CString valid_date;
		valid_date.Format(TEXT("%s.%s.%s-%s.%s.%s"),
			people_valid_start_date.Left(4), people_valid_start_date.Mid(4, 2), people_valid_start_date.Right(2),
			people_valid_end_date.Left(4), people_valid_end_date.Mid(4, 2), people_valid_end_date.Right(2));
		_ctl_valid_date.SetWindowText(valid_date);

		Bitmap* people_photo = Utility::CreateBitmapFromMemory(data->GetBitmapData(), data->GetBitmapSize());
		if (people_photo)
		{
			CRect rect;
			int cx = people_photo->GetWidth(), cy = people_photo->GetHeight();
			GetDlgItem(IDC_PEOPLE_PHOTO)->GetWindowRect(&rect);
			ScreenToClient(&rect);
			Graphics graph(m_hWnd);
			graph.DrawImage(people_photo, rect.left, rect.top, 0, 0, cx, cy, UnitPixel);
			delete people_photo;
		}

		if (_idcard_data)
		{
			delete _idcard_data;
			_idcard_data = nullptr;
		}
		else
		{
			_idcard_data = new IdCardData(data->GetPeopleName(), data->GetPeopleSex(),
				data->GetPeopleNation(), data->GetPeopleBirthday(), data->GetPeopleAddress(),
				data->GetPeopleIdCode(), data->GetSignDepartment(), data->GetValidStartDate(),
				data->GetValidEndDate(), data->GetBitmapData(), data->GetBitmapSize());
		}

		GetDlgItem(IDC_COLLECTION)->EnableWindow();

	} while (false);

	if (data)
	{
		delete data;
		data = nullptr;
	}

	return result;
}

void IdCardMainDlg::OnReadCompleted(IdCardData* data)
{
	SendMessageA(GetSafeHwnd(), WM_REFRESH_IDCARD, (WPARAM)data, 0);
}

void IdCardMainDlg::OnReadError(int error)
{
	BaseLogError("read card error : %d", error);
}

void IdCardMainDlg::DoCollecting()
{
	BaseLogInfo("IdCardCollector enter collect thread");

	boost::shared_ptr<IdCardManager> manager = IdCardManager::GetInstance();
	while (true)
	{
		unsigned int result = WaitForSingleObject(_collection_signal, _timer_second);
		if (result == WAIT_OBJECT_0)
		{
			BaseLogInfo("IdCardCollector collect thread received exit signal, about to exit");
			break;
		}

		BaseLogInfo("IdCardCollector collect thread read card start");
		manager->ReadCard(true, this);
		BaseLogInfo("IdCardCollector collect thread read card end");

		if (_exiting)
		{
			BaseLogInfo("IdCardCollector collect thread received exit signal, about to exit");
			break;
		}
	}

	BaseLogInfo("IdCardCollector leave collect thread");
}

void IdCardMainDlg::FormatDate(std::string& format_date, const char* date, int length)
{
	char year[8] = { 0 };
	char month[8] = { 0 };
	char day[8] = { 0 };
	if (length == 8)
	{
		const char* p = date;
		memcpy(year, p, 4);
		p = p + 4;
		memcpy(month, p, 2);
		p = p + 2;
		memcpy(day, p, 2);
		format_date.append(year);
		format_date.append("-");
		format_date.append(month);
		format_date.append("-");
		format_date.append(day);
	}
	else
	{
		format_date.assign(date);
	}
}

//////////////////////////////////////////////////////////////////////////
void IdCardMainDlg::DoTest()
{
	
}