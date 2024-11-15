#include <iostream>
#include <fstream>
#include <map>
#include <queue>
#include <string>
#include <Windows.h>
#include <algorithm>

using namespace std;

struct Node {
    vector<pair<char, int>> charWFreq;
    Node* left, * right;
};

Node* createNode(vector<pair<char, int>> charWFreq, Node* left, Node* right) {
    Node* node = new Node;
    node->charWFreq = charWFreq;
    node->left = left;
    node->right = right;
    return node;
}

bool comparator(pair<char, int> p1, pair<char, int> p2) {
    return p1.second > p2.second;
}

void growShFTree(Node* node) {

    // Если дошли до листа, то выход из функции
    if (node->charWFreq.size() == 1) {
        return;
    }

    // Подсчет среднего значения частоты в последовательности
    double dS = 0;
    for (auto pair : node->charWFreq) { dS += pair.second; }
    dS /= 2;

    int sumFreq = 0, i = 0;
    vector<pair<char, int>> left_v, right_v;
    if (i == 0 && sumFreq + node->charWFreq[i].second >= dS) {
        sumFreq += node->charWFreq[i].second;
        left_v.push_back(node->charWFreq[i]);
        i++;
    }
    while (sumFreq + node->charWFreq[i].second <= dS) {
        sumFreq += node->charWFreq[i].second;
        left_v.push_back(node->charWFreq[i]);
        i++;
    }
    while (i < node->charWFreq.size()) {
        right_v.push_back(node->charWFreq[i]);
        i++;
    }
    sort(left_v.begin(), left_v.end(), comparator); sort(right_v.begin(), right_v.end(), comparator);

    Node* leftNode = createNode(left_v, nullptr, nullptr);
    Node* rightNode = createNode(right_v, nullptr, nullptr);

    node->left = leftNode;
    node->right = rightNode;

    growShFTree(leftNode);
    growShFTree(rightNode);

}

void encode(Node* root, string current_code, map<char, string>& chars_hcodes) {

    if (root == nullptr) return;

    // Првоеряем, дошли ли мы до листа
    if (!root->left && !root->right) {
        chars_hcodes[root->charWFreq[0].first] = current_code;
        return;
    }

    encode(root->left, current_code + "0", chars_hcodes);
    encode(root->right, current_code + "1", chars_hcodes);

}

void decode(Node* root, int& i, string encoded_text, string& decoded_text) {

    // Првоеряем, дошли ли мы до листа
    if (!root->left && !root->right) {
        decoded_text += root->charWFreq[0].first;
        return;
    }

    i++; // int i = -1 в начале
    if (encoded_text[i] == '0') {
        decode(root->left, i, encoded_text, decoded_text);
    }
    else {
        decode(root->right, i, encoded_text, decoded_text);
    }

}


void printTitle() {
    cout << R"(
      _____  _                                         ______
     / ____|| |                                       |  ____|   
    | (___  | |__    __ _  _ __   _ __    ___   _ __  | |__     __ _  _ __    ___
     \___ \ | '_ \  / _` || '_ \ | '_ \  / _ \ | '_ \ |  __|   / _` || '_ \  / _ \
     ____) || | | || (_| || | | || | | || (_) || | | || |     | (_| || | | || (_) |
    |_____/ |_| |_| \__,_||_| |_||_| |_| \___/ |_| |_||_|      \__,_||_| |_| \___/ 
    c o d i n g                                           
)" << "\n";
}


int main()
{

    // Включаем корректный вывод и ввод символов кириллицы.
    setlocale(LC_ALL, "Russian");
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    printTitle();

    // Читаем текст из консоли/файла по выбору пользователя.
    int m;
    cout << "Выберите способ ввода текста для кодирования\n";
    cout << "1 - ввод с клавиатуры\n";
    cout << "2 - чтение из Текст.txt\n";
    cout << "Ввод: "; cin >> m;

    string text_to_encode;
    if (m == 1) {
        cout << "Введите текст для кодирования: ";
        cin.ignore();
        getline(cin, text_to_encode);
    }
    else if (m == 2) {
        // Читаем текст для кодирования из файла в переменную
        string text_file_path = "Текст.txt";

        text_to_encode = ""; string temp;
        fstream file(text_file_path);
        if (file.is_open()) {
            while (!file.eof()) {
                getline(file, temp);
                text_to_encode = text_to_encode + temp + "\n";
            }
            file.close();
        }
        else {
            cout << "Возникла ошибка при открытии файла с текстом.\n";
        }
    }
    else {
        cout << "Некорректный ввод\n";
        return 0;
    }

    // Считаем частоту символов алфавита сообщения (получаем частотную таблицу)
    map<char, int> chars_freq;
    for (char ch : text_to_encode) {
        chars_freq[ch] += 1;
    }

    // Сортируем пары
    vector<pair<char, int>> chars_freq_v;
    for (auto pair : chars_freq) {
        chars_freq_v.push_back(pair);
    }
    sort(chars_freq_v.begin(), chars_freq_v.end(), comparator);

    // Создаем корень дерева и выращиваем его
    Node* root = createNode(chars_freq_v, nullptr, nullptr);
    growShFTree(root);

    // Получаем словарь char - Huffman code
    map<char, string> chars_hcodes;
    string current_code = "";
    encode(root, current_code, chars_hcodes);
    // Получаем закодированный текст
    string encoded_text = "";
    for (char ch : text_to_encode) { encoded_text += chars_hcodes[ch]; }

    // Получаем раскодированный текст
    string decoded_text = ""; int i = -1;
    while (i < (int)encoded_text.size() - 2) {
        decode(root, i, encoded_text, decoded_text);
    }

    // Выводим все на экран
    cout << "\nТекст для кодирования: " << text_to_encode << "\n";

    cout << "Символ\tЧастота\tКод";
    cout << "\n";
    for (auto pair : chars_freq_v) {
        cout << pair.first << "\t" << pair.second << "\t" << chars_hcodes[pair.first] << "\n";
    }

    cout << "\nЗакодированный текст: " << encoded_text;
    cout << "\n\nРаскодированный текст: " << decoded_text << "\n";

}