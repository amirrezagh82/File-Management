#define  _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <direct.h>
#include <iostream>
#include <string>
#include <conio.h>
#include <fstream>

using namespace std;

enum ConsoleColor { BLACK = 0, DarkBlue, DarkGreen, DarkCyan, DarkRed, DarkMagenta, DarkYellow, Gray, DarkGray, Blue, Green, Cyan, Red, Magenta, Yellow, White };

//this function move cursor to position x and y
void gotoxy(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

bool setTextColor(ConsoleColor foregound, ConsoleColor background) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD color = foregound | (background << (int)4);
	return SetConsoleTextAttribute(hConsole, color);
}

void Clrscr(void)
{
	system("cls");
	setTextColor(Gray, BLACK); //reset color
}

void list_file(void) {
	system("dir /O:GN");
}

string ExePath() {
	char buffer[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	string::size_type pos = string(buffer).find_last_of("\\/");
	return string(buffer).substr(0, pos);
}

// Delete LeftPos to RightPos elements
int deleteElement(char inputArray[], int LeftPos, int RightPos, int ArrayLen) {
	int i, j = 0;
	for (i = 0; i < ArrayLen; i++) {
		if (i <= LeftPos || i >= RightPos) {
			inputArray[j] = inputArray[i];
			j++;
		}
	}
	// Return size of Array after delete element
	return j;
}

void showMenu(std::string cdir, int console_width){
	Clrscr();
	list_file();
	setTextColor(DarkRed, BLACK);
	for (int i = 0; i < console_width; i++)
		cout << "-";
	setTextColor(Cyan, BLACK);
	cout << "\n Key words:";
	setTextColor(DarkCyan, BLACK);
	cout << "\n \texit: <end program> | FileContent: <show file content> | CopyFile: <copy a file> | CopyDir: <cony a directory>";
	cout << "\n \tCutFile: <cut a file> | CutDir: <cut a directory> | CreateFile: <create a file> | CreateDir: <create directory>";
	cout << "\n \tRename: <rename a file or directory> | Info: <show information of file or directory> | Edit: <edit file content>";
	cout << "\n \t..: <go to up directory> ";
	cout << "\n";
	setTextColor(DarkRed, BLACK);
	for (int i = 0; i < console_width; i++)
		cout << "-";
	setTextColor(Green, BLACK);
	cout << "> ";
	setTextColor(Gray, BLACK);

}

int GetConsoleWidth(void) {
	int rows;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	rows = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	return rows;
}

int _cd(char path[128]) { return _chdir(path); }

int Create_Directory(char* DirectoryPath) {
	if (_mkdir(DirectoryPath) != 0) {
		return GetLastError();
	}
	return 0;
}

long GetFileSize(std::string filename)
{
	struct stat stat_buf;
	int rc = stat(filename.c_str(), &stat_buf);
	return rc == 0 ? stat_buf.st_size : -1;
}

int Create_File(char* Filename, char* TargetDirectoryPath) {
	char fPath[1024];
	strcpy(fPath, TargetDirectoryPath);
	strcat(fPath, "/");
	strcat(fPath, Filename);
	//ofstream MyWriteFile(fPath);	
	//MyWriteFile.close(); // Close the file

	fstream mfile;
	mfile.open(fPath, ios::out);
	if (!mfile)
	{
		cout << "Error in creating file!!!";
		return GetLastError();
	}
	mfile.close();
	return 0;
}

int WriteToFile(char* Filepath, char* data, int sizeofdata, bool IsAppend = false) {
	ofstream wf;
	if (IsAppend)
		wf.open(Filepath, ios::out | ios::binary | ios::app);
	else
		wf.open(Filepath, ios::out | ios::binary);

	if (!wf) {
		cout << "Cannot open file!";
		return GetLastError();
	}
	wf.write(data, sizeofdata);
	wf.close();
	if (!wf.good()) {
		cout << "Error occurred at writing time!";
		return GetLastError();
	}
	return 0;
}

int ReadFromFile(char* Filepath, char*& data, long& sizeOfData) {
	long filesize = GetFileSize(Filepath);
	if (filesize == -1)
		return GetLastError();
	else if (filesize == 0) {
		data = NULL;
	}
	else {
		data = new char[filesize];
		ifstream rf(Filepath, ios::out | ios::binary);
		if (!rf) {
			cout << "Cannot open file!";
			return GetLastError();
		}
		rf.read((char*)data, filesize);
		rf.close();
		if (!rf.good()) {
			cout << "Error occurred at reading time!";
			return GetLastError();
		}
	}
	sizeOfData = filesize;
	return 0;
}

void PrintArrayBytes(char* inputData, long size) {
	for (int i = 0; i < size; i++) {
		cout << inputData[i];
	}
}

void RenameFileOrDirectory(char* oldpath, char* newname) {
	char comnd[1024];
	strcpy(comnd, "REN \"");
	strcat(comnd, oldpath);
	strcat(comnd, "\" \"");
	strcat(comnd, newname);
	strcat(comnd, "\" 2>&1");
	system(comnd);
}

void copyDirectory(char* sourcePath, char* destPath) {
	char cmnd[1024];
	strcpy(cmnd, "Xcopy \"");
	strcat(cmnd, sourcePath);
	strcat(cmnd, "\" \"");
	strcat(cmnd, destPath);
	strcat(cmnd, "\" /E /H /C /I");
	system(cmnd);
}

int copyFile(char* sourceFilePath, char* DestFilePath) {
	std::ifstream ifs(sourceFilePath, std::ios::binary | std::ios::in);
	std::ofstream ofs(DestFilePath, std::ios::binary | std::ios::out);
	if (ifs.is_open() == false || ofs.is_open() == false)
		return GetLastError();
	const int len = 4096;
	char buf[4096];
	while (1) {
		if (ifs.eof())
			break;
		ifs.read(buf, len);
		int nBytesRead = ifs.gcount();
		if (nBytesRead <= 0)
			break;
		ofs.write(buf, nBytesRead);
	}
	ifs.close();
	ofs.close();

	return 0;
}

void CutDirectory(char* sourcePath, char* destPath) {
	copyDirectory(sourcePath, destPath);
	char cmnd[1024];
	strcpy(cmnd, "rmdir /s \"");
	strcat(cmnd, destPath);
	strcat(cmnd, "\"");
	system(cmnd);
}

int CutFile(char* sourceFilePath, char* DestFilePath) {
	int errcode = copyFile(sourceFilePath, DestFilePath);
	if (errcode == 0) {
		remove(sourceFilePath);
	}
	else {
		return errcode;
	}
	return 0;
}

int ShowFileDirectoryInfo(char* filepath) {
	struct stat fileInfo;
	if (stat(filepath, &fileInfo) != 0) {
		std::cerr << "Error: " << strerror(errno) << '\n';
		return -1;
	}
	std::cout << "Type:         : ";
	if ((fileInfo.st_mode & S_IFMT) == S_IFDIR) { // From sys/types.h
		std::cout << "Directory\n";
	}
	else {
		std::cout << "File\n";
	}
	// Printing all the details related to the file
	std::cout << "Size          : " <<
		fileInfo.st_size << '\n';               // Size in bytes
	std::cout << "Device        : " <<
		(char)(fileInfo.st_dev + 'A') << '\n';  // Device number
	std::cout << "Created       : " <<
		std::ctime(&fileInfo.st_ctime);         // Creation time
	std::cout << "Modified      : " <<
		std::ctime(&fileInfo.st_mtime);         // Last mod time
	return 0;
}

int ShowFileContent(char* filepath) {
	setTextColor(Gray, BLACK);
	for (int i = 0; i < GetConsoleWidth(); i++)
		cout << "-";
	cout << endl;
	cout << "File \"" << filepath << "\" Informations: " << endl;
	if (ShowFileDirectoryInfo(filepath) == -1) {
		return GetLastError();
	}
	setTextColor(DarkGray, BLACK);
	cout << endl;
	for (int i = 0; i < (GetConsoleWidth() - strlen(" File Content ")) / 2; i++)
		cout << "-";
	cout << " File Content ";
	for (int i = 0; i < (GetConsoleWidth() - strlen(" File Content ")) / 2; i++)
		cout << "-";

	char* data;
	long sz = 0;
	if (ReadFromFile(filepath, data, sz) != 0) {
		return GetLastError();
	}
	PrintArrayBytes(data, sz);
	delete[] data;
	cout << endl;
	for (int i = 0; i < GetConsoleWidth(); i++)
		cout << "_";
	return 0;
}

int EditFileContent(char* FilePath, int pos, char* addData, int addDataSize, char*& ChangedData) {
	char* data;
	long sz = 0;
	if (ReadFromFile(FilePath, data, sz) != 0) {
		return -1;
	}
	if (pos > sz)
		pos = sz;
	int newsize = sz + addDataSize;
	ChangedData = new char[newsize];
	int i = 0;
	for (; i < pos; i++)
		ChangedData[i] = data[i];
	for (int j = 0; j < addDataSize; j++) {
		ChangedData[pos + j] = addData[j];
	}
	for (; i < sz; i++)
		ChangedData[addDataSize + i] = data[i];
	delete[] data;
	return newsize;

}

int EditFileContent(char* FilePath, int pos, int removelen, char*& ChangedData) {
	char* data;
	long sz = 0;
	if (ReadFromFile(FilePath, data, sz) != 0) {
		return -1;
	}
	if (pos > sz || pos + removelen > sz) {
		ChangedData = new char[sz];
		memcpy(ChangedData, data, sz);
		delete[] data;
		return 0;
	}
	int newez = deleteElement(data, pos, pos + removelen + 1, sz);
	ChangedData = new char[newez];
	memcpy(ChangedData, data, newez);
	delete[] data;
	return newez;
}

//showing main menu
void ShowMainMenu() {
	Clrscr();
	gotoxy(40, 10); printf("----------------- Main Menu -----------------");
	setTextColor(Cyan, BLACK);  //set color
	gotoxy(40, 12); printf(" 1] Sign in");
	gotoxy(40, 14); printf(" 2] Register User");
	gotoxy(40, 16); printf(" 3] Exit");
	setTextColor(White, BLACK);
	gotoxy(38, 18); printf("Enter Number Of Menu: ");
	setTextColor(Gray, BLACK);
}

//check username and password (if username=Admin & password=Admin: Secretary profile)
int CheckUserPassword(char* username, char* passwrod) {
	int retValue = 0;
	char userpass[1024];
	strcpy(userpass, username);
	strcat(userpass, ":");
	strcat(userpass, passwrod);
	char* UsersFileContent;
	long sz = 0;
	if (ReadFromFile((char*)"Users", UsersFileContent, sz) != 0)
		return -1;

	if (strstr(UsersFileContent, userpass))
		retValue = 1;
	delete[] UsersFileContent;
	return retValue;
}

int SignInMenu() {
	char username[256];
	char password[256];
	int result = 0;

	setTextColor(Red, Gray);
	gotoxy(40, 10); printf("----------------- Sing In -----------------");
	setTextColor(Cyan, BLACK);
	gotoxy(40, 12); printf(" Please Enter Username: ");
	gotoxy(40, 14); printf(" Please Enter password: ");
	//setTextColor(White, BLACK); //reset color

	setTextColor(Red, BLACK);
	gotoxy(40 + 25, 12); scanf("%s", &username); //getting username from keyboard
	gotoxy(40 + 25, 14); scanf("%s", &password); //getting password from keyboard
	Clrscr();
	return CheckUserPassword(username, password); //checking username and passwords
}

void RegisterMenu() {
	char username[256];
	char password[256];
	int result = 0;

	setTextColor(Red, Gray);
	gotoxy(40, 10); cout << "----------------- Register User -----------------";
	setTextColor(Cyan, BLACK);
	gotoxy(40, 12); cout << " Please Enter Username: ";
	gotoxy(40, 14); cout << " Please Enter Password: ";
	setTextColor(Gray, BLACK);

	setTextColor(Red, BLACK);
	gotoxy(40 + 24, 12); scanf("%s", &username); //getting IDNumber from keyboard
	gotoxy(40 + 24, 14); scanf("%s", &password); //getting Email from keyboard

	char userpass[512];
	strcpy(userpass, username);
	strcat(userpass, ":");
	strcat(userpass, password);
	strcat(userpass, "\n");
	if (WriteToFile((char*)"Users", userpass, strlen(userpass), true) == 0) {
		gotoxy(40, 23);
		setTextColor(Green, BLACK);
		cout << "-] User \"" << username << "\" added!";
	}
	else {
		gotoxy(40, 23);
		setTextColor(Red, BLACK);
		cout << "-] Error: User Not Allow to add!";
	}

	//press any key to return last menu
	setTextColor(Yellow, BLACK);
	gotoxy(38, 25);
	cout << "Press any Key to return...";
	setTextColor(Gray, BLACK); //reset color
	_getch();
	return;
}

void MainPage() {
	string cdir = ExePath();
	showMenu(cdir, GetConsoleWidth());

	char CommandStr[1024];
	while (true) {
		cin.getline(CommandStr, 1024);
		if (strcmp(CommandStr, "") != 0) {
			if (strcmp(CommandStr, "exit") == 0) {
				Clrscr();
				break;
			}
			else if (strcmp(CommandStr, "FileContent") == 0) {
				char filepath[512];
				cout << "\nEnter File Path: ";
				cin.getline(filepath, 512);
				Clrscr();
				ShowFileContent(filepath);
				setTextColor(Yellow, BLACK);
				system("pause");
				setTextColor(Gray, BLACK);
			}
			else if (strcmp(CommandStr, "CopyFile") == 0) {
				//Clrscr();
				char srcpath[512];
				char destpath[512];
				cout << "\nEnter Source Path: ";
				cin.getline(srcpath, 512);
				cout << "\nEnter Dest Path: ";
				cin.getline(destpath, 512);
				int errcode = copyFile(srcpath, destpath);
				if (errcode == 0) {
					setTextColor(Green, BLACK);
					cout << "File \"" << srcpath << "\" Successfully copied to dest \"" << destpath << "\"";
				}
				else {
					setTextColor(Red, BLACK);
					cout << "Copy File failed with error code: \"" << errcode << "\"";
				}
				setTextColor(Yellow, BLACK);
				system("pause");
				setTextColor(Gray, BLACK);
			}
			else if (strcmp(CommandStr, "CopyDir") == 0) {
				//Clrscr();
				char srcpath[512];
				char destpath[512];
				cout << "\nEnter Source Path: ";
				cin.getline(srcpath, 512);
				cout << "\nEnter Dest Path: ";
				cin.getline(destpath, 512);
				copyDirectory(srcpath, destpath);
				setTextColor(Yellow, BLACK);
				system("pause");
				setTextColor(Gray, BLACK);
			}
			else if (strcmp(CommandStr, "CutFile") == 0) {
				char srcpath[512];
				char destpath[512];
				cout << "\nEnter Source Path: ";
				cin.getline(srcpath, 512);
				cout << "\nEnter Dest Path: ";
				cin.getline(destpath, 512);
				int errcode = CutFile(srcpath, destpath);
				if (errcode == 0) {
					setTextColor(Green, BLACK);
					cout << "File \"" << srcpath << "\" Successfully moved to dest \"" << destpath << "\"";
				}
				else {
					setTextColor(Red, BLACK);
					cout << "cut File failed with error code: \"" << errcode << "\"";
				}
				setTextColor(Yellow, BLACK);
				system("pause");
				setTextColor(Gray, BLACK);
			}
			else if (strcmp(CommandStr, "CutDir") == 0) {
				char srcpath[512];
				char destpath[512];
				char yesno[3];
				cout << "\nEnter Source Path: ";
				cin.getline(srcpath, 512);
				cout << "\nEnter Dest Path: ";
				cin.getline(destpath, 512);
				cout << "\nAre You Sure? (y/n): ";
				cin.getline(yesno, 3);
				if (yesno[0] == 'y' || yesno[0] == 'Y') {
					CutDirectory(srcpath, destpath);
				}
				setTextColor(Yellow, BLACK);
				system("pause");
				setTextColor(Gray, BLACK);
			}
			else if (strcmp(CommandStr, "CreateFile") == 0) {
				char fname[512];
				char dirpath[512];
				char yesno[3];
				cout << "\nEnter File Name: ";
				cin.getline(fname, 512);
				cout << "\nEnter Directory Path: ";
				cin.getline(dirpath, 512);
				cout << "\nAre You Sure? (y/n): ";
				cin.getline(yesno, 3);
				if (yesno[0] == 'y' || yesno[0] == 'Y') {
					int errcode = Create_File(fname, dirpath);

					if (errcode == 0) {
						setTextColor(Green, BLACK);
						cout << "File \"" << fname << "\" Successfully Created!";
					}
					else {
						setTextColor(Red, BLACK);
						cout << "Create File failed with error code: \"" << errcode << "\"";
					}
				}
				setTextColor(Yellow, BLACK);
				system("pause");
				setTextColor(Gray, BLACK);
			}
			else if (strcmp(CommandStr, "CreateDir") == 0) {
				char dirpath[512];
				char yesno[3];
				cout << "\nEnter Directory Path: ";
				cin.getline(dirpath, 512);
				cout << "\nAre You Sure? (y/n): ";
				cin.getline(yesno, 3);
				if (yesno[0] == 'y' || yesno[0] == 'Y') {
					int errcode = Create_Directory(dirpath);
					if (errcode == 0) {
						setTextColor(Green, BLACK);
						cout << "File \"" << dirpath << "\" Successfully Created!";
					}
					else {
						setTextColor(Red, BLACK);
						cout << "Create Directory failed with error code: \"" << errcode << "\"";
					}
				}
				setTextColor(Yellow, BLACK);
				system("pause");
				setTextColor(Gray, BLACK);
			}
			else if (strcmp(CommandStr, "Rename") == 0) {
				char path[512];
				char newname[512];
				char yesno[3];
				cout << "\nEnter File or Directory Path: ";
				cin.getline(path, 512);
				cout << "\nEnter new name: ";
				cin.getline(newname, 512);
				cout << "\nAre You Sure? (y/n): ";
				cin.getline(yesno, 3);
				if (yesno[0] == 'y' || yesno[0] == 'Y') {
					RenameFileOrDirectory(path, newname);
				}
				setTextColor(Yellow, BLACK);
				system("pause");
				setTextColor(Gray, BLACK);
			}
			else if (strcmp(CommandStr, "Info") == 0) {
				char path[512];
				cout << "\nEnter File Or Directory Path: ";
				cin.getline(path, 512);
				ShowFileDirectoryInfo(path);
				setTextColor(Yellow, BLACK);
				system("pause");
				setTextColor(Gray, BLACK);
			}
			else if (strcmp(CommandStr, "Edit") == 0) {
				char filepath[512];
				char ans[10];
				cout << "\nEnter File Path: ";
				cin.getline(filepath, 512);
				Clrscr();
				setTextColor(DarkGray, BLACK);
				ShowFileContent(filepath);
				std::cout << "Do you want insert (I/i) or delete (D/d) part?";
				cin.getline(ans, 10);
				if (ans[0] == 'I' || ans[0] == 'i') {
					char pos[512];
					char insdata[2048];
					cout << "\nEnter position of file you want to insert: ";
					cin.getline(pos, 512);
					cout << "\nEnter Data you want to insert: ";
					cin.getline(insdata, 2048);
					char* newData;
					int newsz = EditFileContent(filepath, strlen(pos), insdata, strlen(insdata), newData);
					if (newsz > -1) {
						setTextColor(DarkGray, BLACK);
						cout << "-----------------------New Content*------------------\n";
						PrintArrayBytes(newData, newsz);
						setTextColor(Gray, BLACK);
						cout << "\nDo you want to save changes?(y/n)";
						cin.getline(ans, 10);
						if (ans[0] == 'Y' || ans[0] == 'y') {
							WriteToFile(filepath, newData, newsz);
						}
					}
				}
				else if (ans[0] == 'D' || ans[0] == 'd') {
					char pos[512];
					char len[512];
					cout << "\nEnter position of file you want to remove: ";
					cin.getline(pos, 512);
					cout << "\nEnter length of chars you want to remove: ";
					cin.getline(len, 512);
					char* newData;
					int newsz = EditFileContent(filepath, atoi(pos), atoi(len), newData);
					if (newsz > -1) {
						setTextColor(DarkGray, BLACK);
						cout << "-----------------------New Content*------------------\n";
						PrintArrayBytes(newData, newsz);
						setTextColor(Gray, BLACK);
						cout << "\nDo you want to save changes?(y/n)";
						cin.getline(ans, 10);
						if (ans[0] == 'Y' || ans[0] == 'y') {
							WriteToFile(filepath, newData, newsz);
						}
					}
				}
				setTextColor(Yellow, BLACK);
				system("pause");
				setTextColor(Gray, BLACK);
			}
			else if (_cd(CommandStr) == 0) {
			}
			else {
				setTextColor(DarkRed, BLACK);
				switch (errno)
				{
				case ENOENT:
					cout << "Unable to locate the directory: " << CommandStr;
					break;
				case EINVAL:
					cout << "Invalid buffer.";
					break;
				default:
					cout << "Unknown error.";
				}

				setTextColor(Yellow, BLACK);
				cout << " [Press any Key to return...]";
				_getch();
				setTextColor(Gray, BLACK);
			}
		}
		cdir = ExePath();
		showMenu(cdir, GetConsoleWidth());
	}
}



int main() {
	int menunumber = 0;
	while (1) {
		do {
			setTextColor(Gray, BLACK);
			ShowMainMenu();
			setTextColor(Red, BLACK);
			scanf("%d", &menunumber); //get main menu number from keyboard
			setTextColor(Gray, BLACK);
		} while (menunumber < 1 || menunumber > 3);

		switch (menunumber)
		{
		case 1:
		{
			Clrscr(); //clear screen				
			char uname[512];
			int signres = SignInMenu();
			gotoxy(40, 23);

			if (signres == 0) {
				setTextColor(Red, BLACK);
				printf("-] Error: Username or Password is wrong!");
			}
			else if (signres == -1) {
				setTextColor(Red, BLACK);
				printf("-] Error: in reading Users File");
			}
			else if (signres == 1) {
				Clrscr(); //clear screen
				MainPage();
			}
			//press any key to return last menu
			setTextColor(Yellow, BLACK);
			gotoxy(38, 25);
			cout << "Press any Key to return...";
			setTextColor(Gray, BLACK); //reset color
			_getch();
			break;
		}
		case 2:
			Clrscr(); //clear screen
			RegisterMenu();
			break;
		case 3:
			Clrscr();
			return 0;
			break;
		}

	}
	return 0;
}

