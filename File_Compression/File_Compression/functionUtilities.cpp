#include "functionUtilities.h"

//check the path to encoding file
bool checkInPathEnFormat(string inPath, string& outPath) {
	string format = inPath.substr(inPath.size() - 4, inPath.size());//take the end of path: .txt

	if (format != ".txt") return false;//format of file is wrong

	outPath = inPath.substr(0, inPath.size() - 4) + ".enFi";//change tail
	return true;
}

//check the path to decoding file
bool checkInPathDeFormat(string inPath, string& outPath, int format=0) {
	string encode;
	string decode;
	
	if (format == 0) {
		encode = ".enFi";
		decode = ".deFi";
	}
	else {
		encode = ".enFo";
		decode = ".deFo";
	}
	string tail = inPath.substr(inPath.size() - 5, inPath.size());//take the end of path: .enFi

	if (tail != encode) 
		return false;//format of file is wrong

	outPath = inPath.substr(0, inPath.size() - 5) + decode;//change tail
	return true;
}
//check if file can open
bool checkInPathOpen(string inPath) {
	ifstream in(inPath);

	if (!in) {//cannot open file
		return false;
	}

	in.close();
	return true;
}
//encoding file
void encodingFile() {

	cout << "Enter the path to the Text File needed to encode :";

	string inPath;
	string outPath;
	cin >> inPath;

	cout << "Encoding..." << endl;
	clock_t start = clock();
	encodingFile(inPath);
	clock_t end = clock();
	cout << "Time: " << (end - start) / CLOCKS_PER_SEC <<" s"<< endl;
	cout << "Encoding Successfully" << endl;
}

//decoding file
void decodingFile() {

	cout << "Enter the path to the Text File needed to decode (.enFi):";

	string inPath;
	string outPath;
	cin >> inPath;

	if (!checkInPathDeFormat(inPath, outPath)) {
		cout << "File have no right Format" << endl;
		return;
	}

	if (!checkInPathOpen(inPath)) {
		cout << "File not Found" << endl;
		return;
	}

	cout << "Decoding..." << endl;
	clock_t start = clock();
	decodingFile(inPath);
	clock_t end = clock();
	cout << "Time: " << (end - start) / CLOCKS_PER_SEC << " s" << endl;
	cout << "Decoding Succesfully" << endl;
}
//encoding file
void encodingFile(string inPath) {
	if (!checkInPathOpen(inPath)) {
		cout << "File not Found" << endl;
		exit(0);
	}

	string delimiter = ".";
	string outPath = inPath.substr(0, inPath.find_last_of(delimiter));
	outPath = outPath + ".enFi";
	string fileTail = inPath.substr(inPath.find_last_of(delimiter), inPath.size() - 1);

	if (fileTail == ".txt") {
		cout << "Huffman encoding..." << endl;
		huffman algorithm(inPath, outPath);
		algorithm.encodedFile();
	}
	else {
		cout << "LZ77 encoding..." << endl;
		LZ77* lz77 = new LZ77(inPath, outPath);
		lz77->compress();
		delete lz77;
	}
}

//decoding file
void decodingFile(string inPath) {
	if (!checkInPathOpen(inPath)) {
		cout << "File not Found" << endl;
		exit(0);
	}

	ifstream checkFile(inPath, ios::in);
	string check;
	getline(checkFile, check);
	checkFile.close();

	if (check.find(":\\") == string::npos) {
		cout << "Huffman decoding..." << endl;
		string delimiter = ".";
		string outPath = inPath.substr(0, inPath.find_last_of(delimiter));
		outPath = outPath + ".txt";

		huffman algorithm(inPath, outPath);
		algorithm.decodedFile();
	}
	else {
		cout << "LZ77 decoding..." << endl;
		LZ77* lz77 = new LZ77(inPath);
		lz77->decompress();

		delete lz77;
	}
}
//transfer string to char
char* transStringToChar(string s) {
	int size = s.size();
	char* result = new char[size];

	int i = 0;
	while (s[i]) {
		result[i] = s[i];
		i++;
	}
	result[i] = '\0';

	return result;
}
//copy to the new folder has the same as content
string copyFolder(string inPath, int format=0) {
	string encode = ".enFo";
	string decode = ".deFo";
	string outPath;

	if (format == 0)
		outPath = inPath + encode;
	else
		outPath = inPath.substr(0, inPath.size()-5) + decode;

	inPath += '\0';
	SHFILEOPSTRUCT sf;
	memset(&sf, 0, sizeof(sf));
	sf.hwnd = 0;
	sf.wFunc = FO_COPY;
	sf.pFrom = (LPCSTR)inPath.c_str();
	sf.pTo = (LPCSTR)outPath.c_str();

	sf.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI;
	int n = SHFileOperation(&sf);
	if (n != 0) {
		cout << "Cann't copy folder" << endl;
	}
	return outPath;
}
//processing interacting with folder
void processFolder(string path, int format=0){
	DIR* dir;
	struct dirent* ent;

	const char* cPath = path.c_str();
	dir = opendir(cPath);

	void(*function)(string);
	string tailFile;
	//idenfify tail
	if (format == 0) {
		function = encodingFile;
	}	
	else
	{
		function = decodingFile;
		tailFile = ".enFi";
	}

	if (dir != NULL) {//access the path
		while ((ent = readdir(dir)) != NULL) {
			string filename = ent->d_name;
			if (filename == "." || filename == "..")
				continue;
			if (filename.size() > 4 && filename.find(".") != string::npos) {
				string tail;
				if (format == 1) {//compression
					tail = filename.substr(filename.size() - 5, filename.size());//check if the it is the name of the file
					if (tail == tailFile) {//find the right format file
						string newPath = path + "\\" + filename;
						function(newPath);//process

						if (remove(transStringToChar(newPath))) {//remove the beggin file
							cout << "Cann't remove the old File" << endl;
						}

						continue;
					}
				}
				else {//decompress
					string newPath = path + "\\" + filename;
					function(newPath);
					if (remove(transStringToChar(newPath))) {//remove the beggin file
						cout << "Cann't remove the old File" << endl;
					}
					continue;
				}
			}

			string newPath = path + "\\" + filename;//go deeply into the folder
			if (format == 0)
				processFolder(newPath, 0);//compression
			else
				processFolder(newPath, 1);//decompression

		}
		closedir(dir);
	}
}

//check the path to folder
bool checkPathFolder(string inPath) {
	DIR* dir;//direction

	char* cPath = transStringToChar(inPath);
	dir = opendir(cPath);

	if (dir == NULL) {//cann't open folder
		return false;
	}

	closedir(dir);
	return true;
}
//encoding folder
void encodingFolder() {
	string inPath;
	string outPath;

	cout << "Enter the path to the Folder containing Text File needed to encode:";
	rewind(stdin);
	getline(cin, inPath);

	if (!checkPathFolder(inPath)) {
		cout << "Cann't open the Folder" << endl;
		exit(0);
	}

	cout << "Encoding..." << endl;
	clock_t start = clock();
	outPath = copyFolder(inPath);//copy to the new folder has the same content
	processFolder(outPath);//Encoding these folder
	clock_t end = clock();
	cout << "Time: " << (end - start) / CLOCKS_PER_SEC << " s" << endl;
	cout << "Encoding Successfully" << endl;
}

void decodingFolder() {
	string inPath;
	string outPath;

	cout << "Enter the path to the Folder containing Text File needed to decode (.enFo):";
	rewind(stdin);
	getline(cin, inPath);


	if (!checkPathFolder(inPath)) {
		cout << "Cann't open the Folder" << endl;
		exit(0);
	}

	if (!checkInPathDeFormat(inPath, outPath,1)) {
		cout << "Folder has no right format" << endl;
		exit(0);
	}

	cout << "Decoding..." << endl;
	clock_t start = clock();
	outPath = copyFolder(inPath, 1);//copy to the new folder has the same content
	processFolder(outPath, 1);//Decoding these folder
	clock_t end = clock();
	cout << "Time: " << (end - start) / CLOCKS_PER_SEC << " s" << endl;
	cout << "Decoding Successfully" << endl;
}