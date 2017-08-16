#include <Windows.h>
#include <vector>
using namespace std;
typedef struct
{
	int x;
	int y;
}Point;
typedef struct
{
	Point point;
	int penIndex;
}Trace;

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	static WCHAR szWndClassName[] = L"paintwindow";	//�������������ڴ���������
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;		//����������

	wndclass.style = CS_HREDRAW | CS_VREDRAW;		//�������񣺴�����ˮƽ�ʹ�ֱ���� ���ػ�
	wndclass.lpfnWndProc = WndProc;						//���ڹ��̺���ָ��
	wndclass.cbClsExtra = 0;										//����ռ䣬����
	wndclass.cbWndExtra = 0;										//ͬ��
	wndclass.hInstance = hInstance;							//���ھ������WinMain������ȡ
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);	//����ͼ��
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);	//���ڹ��
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	//������ˢ
	wndclass.lpszMenuName = NULL;							//�˵���
	wndclass.lpszClassName = szWndClassName;		//��������

	if (!RegisterClass(&wndclass))	/*ע�ᴰ����*/
	{
		MessageBox(NULL, L"ע��ʧ��", L"����", MB_ICONERROR);
		return 0;
	}

	/*�������ڣ����������ھ��*/
	hwnd = CreateWindow(
		szWndClassName,		//window class name
		L"����",				//window caption
		WS_OVERLAPPEDWINDOW,		//window style
		CW_USEDEFAULT,		//initial x position
		CW_USEDEFAULT,		//initial y position
		CW_USEDEFAULT,		//initial x size
		CW_USEDEFAULT,		//initial y size
		NULL,						//parent window handle
		NULL,						//window menu handle
		hInstance,					//program instance handle
		NULL						//creation paraneters
	);

	ShowWindow(hwnd, nCmdShow);		//��ʾ����
	UpdateWindow(hwnd);						//�涨���ƣ����´���

	while (GetMessage(&msg, NULL, 0, 0))		//��Ϣ����
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);			//�ַ���Ϣ������Ϣ����ϵͳ��ϵͳ�ٴ���WndProc���ڹ���
	}
	return msg.wParam;		//WM_QUIT :  Windows Message Quit
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)		/*���ڹ��̺�����������Ӧ��Ϣ������ϵͳ����*/
{
	HDC hdc;
	PAINTSTRUCT ps;
	
	RECT rect;
	static Point prePoint;		//ǰһ����
	static HPEN hpen[3];
	static int index;
	static vector<Trace> trace;	//��¼�켣���ػ�

	switch (uMsg)
	{
	case WM_CREATE:		//���ڴ���
	{
		hpen[0] = CreatePen(PS_SOLID, 4, RGB(255, 0, 0));		//��������
		hpen[1] = CreatePen(PS_SOLID, 6, RGB(0, 255, 0));
		hpen[2] = CreatePen(PS_SOLID, 8, RGB(0, 0, 255));
	}
	return 0;

	case WM_LBUTTONDOWN:		//����������ʱȡǰ��
	{
		prePoint.x = LOWORD(lParam);
		prePoint.y = HIWORD(lParam);
		Trace temp;
		temp.point = prePoint;
		temp.penIndex = index;
		trace.push_back(temp);
	}
	return 0;

	case WM_MOUSEMOVE:
	{
		if (MK_LBUTTON&wParam)		//λ����ȡָ��λ���ж�������������
		{
			Point point;
			point.x = LOWORD(lParam);
			point.y = HIWORD(lParam);
			hdc = GetDC(hwnd);		//�����豸��������
			
			SelectObject(hdc, hpen[index]);
			MoveToEx(hdc, prePoint.x, prePoint.y, NULL);
			LineTo(hdc, point.x, point.y);
			
			Trace temp;
			temp.point = point;
			temp.penIndex = index;
			trace.push_back(temp);

			ReleaseDC(hwnd, hdc);
			prePoint = point;
		}
	}
		return 0;

	case WM_RBUTTONDOWN:
	{
		index++;
		if (index >= 3)
			index = 0;
	}
	return 0;

	case WM_KEYDOWN:
		if (VK_RETURN == wParam)
		{
			InvalidateRect(hwnd, NULL, true);			//ʹ��������������Ч���Դ��� case WM_PAINT �ػ�
		}
		return 0;

	case WM_PAINT:		//���ڱ����ػ��ʱ��:���/��С��/�������ڴ�С��ʱ����ɺ�������Ϣ���е�WM_PAINT
		hdc = BeginPaint(hwnd, &ps);		//�����豸�������� (��Ч����)
		GetClientRect(hwnd, &rect);			//���ڿͻ�������
		/*��ʾ�ַ�*/
		DrawText(hdc, L"��ʾ������滭���Ҽ�����", -1, &rect, DT_SINGLELINE | DT_TOP | DT_CENTER);
		/*��ͼ����*/
		Point prePoint;
		Point point;
		if (trace.size() > 0)
		{
			prePoint = trace[0].point;
		}
		for(int i=1;i<trace.size();i++)
		{
			point = trace[i].point;
			SelectObject(hdc, hpen[trace[i].penIndex]);
			MoveToEx(hdc, prePoint.x, prePoint.y, NULL);
			LineTo(hdc, point.x, point.y);
			prePoint = point;
		}
		EndPaint(hwnd, &ps);

		return 0;	//ֻҪ��������Ϣ���ͷ���0

	case WM_DESTROY:	//����رյ�ʱ��
		DeleteObject(hpen[0]);		//ɾ������
		DeleteObject(hpen[1]);
		DeleteObject(hpen[2]);

		PostQuitMessage(0);		//����Quit��Ϣ����Ϣ����
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);		//������Ϣ��Ĭ�ϴ��ڹ��̴��������϶���ʱ��
}