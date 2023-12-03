#include"header.h"
#include<filesystem>
struct Email {
	string from;
	string subject;
	string content;
};
std::string getSenderEmailFromEmail(const std::string& emailContent);
std::string getSubjectFromEmail(const std::string& emailContent);
// Hàm di chuyển email vào thư mục
void moveToFolder(const Email& email, const string& folder)
{
	fstream output;
	string s = "emailCount" + folder + ".txt";
	fstream input;
	input.open(s,ios::in);
	int cnt;
	input >> cnt;
	input.close();
	output.open(s, ios::out);
	output << cnt + 1; output.close();

	s = to_string(cnt+1) + ".<chuadoc>" + "<" + email.from + "><" + email.subject + ">.txt";
	
	ofstream outputfile(s);
	outputfile << email.content;
	outputfile.close();
	s = "email" + folder+".txt";
	output.open(s, ios::app);
	output << to_string(cnt + 1) + ".<chuadoc>" + "<" + email.from + "><" + email.subject + ">.txt"<<endl;
	output.close();
}
// Hàm kiểm tra và áp dụng các quy tắc lọc
void applyFilters(const Email& email) {
	// Filter 1: From
	if (email.from == "ahihi@testing.com" || email.from == "ahuu@testing.com") {
		moveToFolder(email, "PROJECT");
		return;  // Không cần kiểm tra các filter khác nếu đã áp dụng filter này
	}

	// Filter 2: Subject
	std::vector<std::string> subjectFilters = { "urgent", "ASAP" };
	for (const auto& filter : subjectFilters) {
		if (email.subject.find(filter) != std::string::npos) {
			moveToFolder(email, "IMPORTANT");
			return;
		}
	}

	// Filter 3: Content
	std::vector<std::string> contentFilters = { "report", "meeting" };
	for (const auto& filter : contentFilters) {
		if (email.content.find(filter) != std::string::npos) {
			moveToFolder(email, "WORK");
			return;
		}
	}

	// Filter 4: Spam
	std::vector<std::string> spamFilters = { "virus", "hack", "crack" };
	for (const auto& filter : spamFilters) {
		if (email.subject.find(filter) != std::string::npos || email.content.find(filter) != std::string::npos) {
			moveToFolder(email, "SPAM");
			return;
		}
	}

	// Nếu không trùng khớp với bất kỳ filter nào, có thể xử lý mặc định hoặc bỏ qua
	moveToFolder(email, "INBOX");
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
	input.open("numemail.txt",ios::in);
	int soluongfilehientai;
	input >> soluongfilehientai;


	for (int i = soluongfilehientai + 1; i <= emailCount; i++) {
		string k = "RETR " + to_string(i) + "\r\n";
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
		Email email;
		email.content = emailContent;
		email.from = getSenderEmailFromEmail(emailContent);
		email.subject = getSubjectFromEmail(emailContent);
		cout << email.from << " " << email.subject << endl;
		applyFilters(email);
	}

	
	input.close();
	fstream output;
	output.open("numemail.txt"/*, ios::out*/);
	output << emailCount;
	output.close();

	const char* quitCommand = "QUIT\r\n";// Gửi lệnh QUIT để đóng kết nối
	send(clientsocket, quitCommand, strlen(quitCommand), 0);
	recv(clientsocket, buffer, sizeof(buffer), 0);
}
// Hàm lấy địa chỉ email người gửi từ nội dung email
std::string getSenderEmailFromEmail(const std::string& emailContent) {
	// Sử dụng biểu thức chính quy để tìm kiếm địa chỉ email trong trường "From"
	std::regex regexPattern(R"(From\s*:\s*([^<>\n]+<([^<>]+)>)|From\s*:\s*([^<>\n]+))");
	std::smatch matches;

	if (std::regex_search(emailContent, matches, regexPattern)) {
		// matches[2] chứa thông tin về địa chỉ email (nếu có)
		// matches[3] chứa thông tin về địa chỉ email (nếu không có dấu ngoặc lẻ)
		if (!matches[2].str().empty()) {
			return matches[2].str(); // Nếu có dấu ngoặc lẻ
		}
		else if (!matches[3].str().empty()) {
			return matches[3].str(); // Nếu không có dấu ngoặc lẻ
		}
	}

	return "Unknown Sender Email";
}
// Hàm lấy chủ đề của email từ nội dung email
std::string getSubjectFromEmail(const std::string& emailContent) {
	// Sử dụng biểu thức chính quy để tìm kiếm trường "Subject"
	std::regex regexPattern(R"(Subject\s*:\s*([^\n]+))");
	std::smatch matches;

	if (std::regex_search(emailContent, matches, regexPattern)) {
		// matches[1] chứa thông tin về chủ đề
		return matches[1].str();
	}

	return "Unknown Subject";
}