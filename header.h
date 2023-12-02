#include<iostream>
#include<WinSock2.h>
#include<ws2tcpip.h>
#include<string>
#include<ctime>
#include<sstream>
#include<unordered_set>
#include<fstream>
#include<vector>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
struct sendmail
{
	string subject="";// chu de email
	string content="";// noi dung email
	string to = "";// gui vs to
	string CC = "";// gui vs cc
	string BCC = "";// gui vs bcc
	int enclosefile;// 1.dinh kem file, 2.khong dinh kem file
	int numfile = 0;//so luong file
	string* a;
};
// Hàm tạo Message-ID duy nhất
string generateMessageID()
{
	time_t currentTime = time(nullptr);
	return "Message-ID: <" + to_string(currentTime) + "@gmail.com>\r\n";
}

string getVietnamDateTime()
{
	time_t rawtime;
	struct tm timeinfo;
	char buffer[80];

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime); // Sử dụng localtime_s thay vì gmtime_s

	strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S +0700", &timeinfo);

	return "Date: " + std::string(buffer) + "\r\n";
}

void setupDLL(int& wsaerr)
{
	/*cout << "Step1: Set up DLL" << endl;*/
	WSADATA wsaData;
	WORD wVersionRequest = MAKEWORD(2, 2);
	wsaerr = WSAStartup(wVersionRequest, &wsaData);
	if (wsaerr != 0)
	{
		cout << "The winsock dll not found!!" << endl;
		return;
	}
	/*else
	{
		cout << "The winsock dll  found!!" << endl;
		cout << "The status: " << wsaData.szSystemStatus << endl;
	}*/
}
void inputsendemail(sendmail& sm)
{
	//to: nhan@gmail.com phong@gmail.com
	//CC: nhan@gmail.com nha@gmail.com
	//BCC:nhan@gmail.com 
	cout << " Day la thong tin soan email:(neu khong dien vui long nhan enter de bo qua)" << endl;
	cin.ignore();
	cout << "To: ";
	getline(cin, sm.to);
	cout << "CC: ";
	getline(cin, sm.CC);
	cout << "BCC: ";
	getline(cin, sm.BCC);
	cout << "Subject: ";
	getline(cin, sm.subject);
	cout << "Content: ";
	getline(cin, sm.content);
	cout << "Co gui kem file khong:(1.co, 2. khong) ";
	cin >> sm.enclosefile;
	if (sm.enclosefile == 1)
	{

		cout << "So luong file muon gui:";
		cin >> sm.numfile;
		sm.a = new string[sm.numfile];
		cin.ignore();
		for (int i = 1; i <= sm.numfile; i++)
		{
			cout << "Cho biet duong dan file thu " << i << ": ";
			getline(cin, sm.a[i]);
		}
	}
}
void sendemail(SOCKET clientsocket,sendmail sm)
{

	// Replace with the actual email content
	string s1 = generateMessageID();
	s1 += getVietnamDateTime();
	s1 += "MIME-Version: 1.0\r\n";
	s1 += "User-Agent: Mozilla Win11\r\n";
	s1 += "Content-Language: en-US";
	if (sm.to != "")
		s1 += "\r\nTo: " + sm.to;
	if (sm.CC != "")
		s1 += "\r\nCc: " + sm.CC;
	if(sm.CC==""&&sm.to==""&&sm.BCC!="")
		s1 += "\r\nTo: undisclosed-recipients:;";
	s1 += "\r\nFrom: Tran Ngoc Nhan <nhancc@gmail.com>\r\n";
	s1 += "Subject: " +sm.subject+"\r\n\r\n";
	s1 += sm.content;
	const char* emailMessage = s1.c_str();

	// Sending the email message
	const char* command = "EHLO [127.0.0.1]\r\n";
	send(clientsocket, command, strlen(command), 0);
	recv(clientsocket, nullptr, 0, 0); // Receive server response

	command = "MAIL FROM: <nhancc@gmail.com>\r\n";
	send(clientsocket, command, strlen(command), 0);
	recv(clientsocket, nullptr, 0, 0);
	
	std::unordered_set<std::string> addedEmails;

	// Gửi email với To
	stringstream ssTo(sm.to);
	string wordTo;
	while (ssTo >> wordTo) {
		if (addedEmails.insert(wordTo).second) { // Chỉ thêm vào nếu chưa tồn tại
			string emailTo = "RCPT TO: <" + wordTo + ">\r\n";
			const char* emailToCommand = emailTo.c_str();
			send(clientsocket, emailToCommand, strlen(emailToCommand), 0);
			recv(clientsocket, nullptr, 0, 0);
		}
	}

	// Gửi email với CC
	if (!sm.CC.empty()) {
		stringstream ssCC(sm.CC);
		string wordCC;
		while (ssCC >> wordCC) {
			if (addedEmails.insert(wordCC).second) { // Chỉ thêm vào nếu chưa tồn tại
				string emailCC = "RCPT TO: <" + wordCC + ">\r\n";
				const char* emailCCCommand = emailCC.c_str();
				send(clientsocket, emailCCCommand, strlen(emailCCCommand), 0);
				recv(clientsocket, nullptr, 0, 0);
			}
		}
	}

	// Gửi email với BCC
	if (!sm.BCC.empty()) {
		stringstream ssBCC(sm.BCC);
		string wordBCC;
		while (ssBCC >> wordBCC) {
			if (addedEmails.insert(wordBCC).second) { // Chỉ thêm vào nếu chưa tồn tại
				string emailBCC = "RCPT TO: <" + wordBCC + ">\r\n";
				const char* emailBCCCommand = emailBCC.c_str();
				send(clientsocket, emailBCCCommand, strlen(emailBCCCommand), 0);
				recv(clientsocket, nullptr, 0, 0);
			}
		}
	}
	command = "DATA\r\n";
	send(clientsocket, command, strlen(command), 0);
	recv(clientsocket, nullptr, 0, 0);

	send(clientsocket, emailMessage, strlen(emailMessage), 0);
	command = "\r\n\r\n.\r\n";
	send(clientsocket, command, strlen(command), 0);
	recv(clientsocket, nullptr, 0, 0);

	cout << "Email message sent successfully" << endl;
}


// Hàm tách địa chỉ email từ dòng "From: ..."
string extractSenderEmail(const string& fromLine) {
	size_t start = fromLine.find('<');
	size_t end = fromLine.find('>', start);

	if (start != string::npos && end != string::npos) {
		return fromLine.substr(start + 1, end - start - 1);
	}

	return ""; // Trả về chuỗi rỗng nếu không tìm thấy
}

// Hàm tách thông tin chủ đề từ chuỗi email
string extractSubject(const string& subjectLine) {
	// Bỏ qua "Subject: " và trả về phần còn lại
	return subjectLine.substr(strlen("Subject: "));
}
// Hàm để kiểm tra xem email có chứa file đính kèm không
bool hasAttachments(const std::string & emailData) {
	return emailData.find("Content-Disposition: attachment") != std::string::npos;
}

// Hàm để tải và lưu trữ file đính kèm
void downloadAttachment(const std::string& attachmentData, const std::string& filename) {
	// Trong thực tế, bạn cần xử lý dữ liệu đính kèm và lưu nó vào tệp filename
	// Ở đây, chúng ta sử dụng ofstream để lưu nội dung file xuống máy cục bộ
	std::ofstream file(filename, std::ios::binary);
	file << attachmentData;
	file.close();

	std::cout << "Downloaded attachment: " << filename << std::endl;
}
// Hàm để lấy tên file từ chuỗi Content-Disposition
std::string getFilenameFromContentDisposition(const std::string& contentDisposition) {
	std::string filename;

	// Tìm vị trí của "filename="
	size_t filenamePos = contentDisposition.find("filename=");

	if (filenamePos != std::string::npos) {
		// Bỏ qua "filename=" và dấu nháy kép mở
		filenamePos += strlen("filename=");
		size_t startPos = contentDisposition.find("\"", filenamePos);

		if (startPos != std::string::npos) {
			// Tìm vị trí của dấu nháy kép đóng
			size_t endPos = contentDisposition.find("\"", startPos + 1);

			if (endPos != std::string::npos) {
				// Lấy phần tên file trong dấu nháy kép
				filename = contentDisposition.substr(startPos + 1, endPos - startPos - 1);
			}
		}
	}

	return filename;
}
void test(string s)
{
	// Kiểm tra xem email có chứa file đính kèm không
	if (hasAttachments(s)) {
		// Tìm vị trí của phần thân file đính kèm
		size_t attachmentStart = s.find("Content-Disposition: attachment");
		size_t attachmentEnd = s.find("--------------", attachmentStart);

		if (attachmentStart != std::string::npos && attachmentEnd != std::string::npos) {
			// Trích xuất nội dung của file đính kèm
			std::string attachmentData = s.substr(attachmentStart, attachmentEnd - attachmentStart);

			// Tên file đính kèm (lấy từ thông tin Content-Disposition)
			std::string filename = "example.txt"; // Thay đổi tên file tùy theo thông tin thực tế

			// Tải và lưu trữ file đính kèm
			downloadAttachment(attachmentData, filename);
		}
	}
	else {
		std::cout << "Email does not have attachments." << std::endl;
	}
}


void POP(SOCKET clientsocket)
{
	char buffer[1024];
	recv(clientsocket, buffer, sizeof(buffer), 0);// Đọc và hiển thị phản hồi từ máy chủ

	const char* userCommand = "USER nhantran57@gmail.com\r\n";// Gửi lệnh USER
	send(clientsocket, userCommand, strlen(userCommand), 0);
	recv(clientsocket, buffer, sizeof(buffer), 0);

	const char* passCommand = "PASS 0909121603nhan\r\n";// Gửi lệnh PASS
	send(clientsocket, passCommand, strlen(passCommand), 0);
	recv(clientsocket, buffer, sizeof(buffer), 0);

	const char* statCommand = "STAT\r\n";// Gửi lệnh LIST để lấy danh sách email
	send(clientsocket, statCommand, strlen(statCommand), 0);
	recv(clientsocket, buffer, sizeof(buffer), 0);
	// Lấy số lượng email từ phản hồi
	int emailCount;
	sscanf_s(buffer, "+OK %d", &emailCount);

	const char* listCommand = "LIST\r\n";// Gửi lệnh LIST để lấy danh sách email
	send(clientsocket, listCommand, strlen(listCommand), 0);
	recv(clientsocket, buffer, sizeof(buffer), 0);
	fstream input;
	input.open("count.txt");
	int soluongfilehientai;
	input >> soluongfilehientai;
	cout << soluongfilehientai;
	
	fstream output1;
	output1.open("email.txt", ios::app);


	
	for (int i = soluongfilehientai+1; i <= emailCount; i++) {
		string k = "RETR " + to_string(i)+"\r\n";
		const char* retrCommand = k.c_str();// Gửi lệnh RETR để lấy nội dung của email thứ nhất
		send(clientsocket, retrCommand, strlen(retrCommand), 0);
		// Nhận dữ liệu cho đến khi gặp "\r\n.\r\n" (kết thúc của email)
		string emailContent;
		while (true)
		{
			recv(clientsocket, buffer, sizeof(buffer), 0);
			emailContent += buffer;

			// Kiểm tra xem chuỗi đã kết thúc email chưa
			size_t pos = emailContent.find("\r\n.\r\n");
			if (pos != string::npos)
			{
				emailContent.resize(pos); // Loại bỏ phần dư thừa sau kết thúc email
				break;
			}
		}
		output1 << emailContent << endl << "--------------------------------------" << endl;
	}

	const char* quitCommand = "QUIT\r\n";// Gửi lệnh QUIT để đóng kết nối
	send(clientsocket, quitCommand, strlen(quitCommand), 0);
	recv(clientsocket, buffer, sizeof(buffer), 0);
	//cap nhat so luong email hien tai
	input.close();
	fstream output;
	output.open("count.txt"/*, ios::out*/);
	output << emailCount;
	output.close();
}