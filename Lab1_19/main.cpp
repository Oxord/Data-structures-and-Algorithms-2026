// Лабораторная 1, вариант 19, Каракулов Илья, ПС-21, VS Code 2022

/*
Имеется файл с текстом на русском языке  без  переносов
слов.  Дать все варианты переноса всех слов текста.  Например,
все варианты  переноса  слова "барабан"  можно представить как
ба-р-а-б-ан. Перенос возможен по следующим правилам:
  1) переносится  либо  остается в конце строки не менее двух
символов;
  2) невозможен перенос перед буквами 'ь' и 'ъ';
  3) слово должно иметь не менее двух слогов;
  4) в  оставшейся  и  переносимой  частях  слова должны быть
гласные буквы (8).
*/

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

bool isRussianLetter(unsigned char c);
bool isVowel(unsigned char c);
bool isSoftOrHardSign(unsigned char c);
bool isValidCutPoint(const string& word, size_t i);
string hyphenate(const string& word);

int main(int argc, char* argv[]) {
    string input, output;

    if (argc >= 3) {
        input = argv[1];
        output = argv[2];
    } else {
        cout << "Введите имя входного файла: ";
        getline(cin, input);
        cout << "Введите имя выходного файла: ";
        getline(cin, output);
    }

    ifstream fin(input, ios::binary);
    if (!fin.is_open()) {
        cerr << "Ошибка: не удалось открыть входной файл \"" << input << "\"" << endl;
        return 1;
    }

    ofstream fout(output, ios::binary);
    if (!fout.is_open()) {
        cerr << "Ошибка: не удалось открыть выходной файл \"" << output << "\"" << endl;
        return 1;
    }

    string word;
    char ch;
    while (fin.get(ch)) {
        unsigned char uch = static_cast<unsigned char>(ch);
        if (isRussianLetter(uch)) {
            word.push_back(ch);
        } else {
            if (!word.empty()) {
                fout << hyphenate(word);
                word.clear();
            }
            fout.put(ch);
        }
    }
    if (!word.empty()) {
        fout << hyphenate(word);
    }

    fin.close();
    fout.close();

    cout << "Готово. Результат записан в файл \"" << output << "\"" << endl;
    return 0;
}

bool isRussianLetter(unsigned char c) {
    return (c >= 0xC0 && c <= 0xFF) || c == 0xA8 || c == 0xB8;
}

// Коды гласных букв в CP1251
const unsigned char VOWEL_CODES[] = {
    0xC0, 0xC5, 0xC8, 0xCE, 0xD3, 0xDB, 0xDD, 0xDE, 0xDF, // А Е И О У Ы Э Ю Я
    0xE0, 0xE5, 0xE8, 0xEE, 0xF3, 0xFB, 0xFD, 0xFE, 0xFF, // а е и о у ы э ю я
    0xA8, 0xB8                                            // Ё ё
};
const size_t VOWEL_CODES_COUNT = sizeof(VOWEL_CODES) / sizeof(VOWEL_CODES[0]);

bool isVowel(unsigned char c) {
    for (size_t i = 0; i < VOWEL_CODES_COUNT; i++) {
        if (VOWEL_CODES[i] == c) {
            return true;
        }
    }
    return false;
}

bool isSoftOrHardSign(unsigned char c) {
    // Ъ    ъ     Ь    ь
    return c == 0xDA || c == 0xFA || c == 0xDC || c == 0xFC;
}

bool isValidCutPoint(const string& word, size_t i) {
    size_t n = word.size();

    // Слева и справа должно остаться минимум по 2 символа
    if (i < 2 || i + 2 > n) {
        return false;
    }

    // Нельзя переносить перед ь/ъ
    if (isSoftOrHardSign(static_cast<unsigned char>(word[i]))) {
        return false;
    }

    // В оставшейся левой части должна быть гласная
    bool leftHasVowel = false;
    for (size_t k = 0; k < i; k++) {
        if (isVowel(static_cast<unsigned char>(word[k]))) {
            leftHasVowel = true;
            break;
        }
    }
    if (!leftHasVowel) {
        return false;
    }

    // В переносимой части должна быть гласная
    bool rightHasVowel = false;
    for (size_t k = i; k < n; k++) {
        if (isVowel(static_cast<unsigned char>(word[k]))) {
            rightHasVowel = true;
            break;
        }
    }

    return rightHasVowel;
}

// Возвращает слово с расставленными дефисами во всех допустимых точках переноса.
string hyphenate(const string& word) {
    size_t n = word.size();
    string result;

    for (size_t j = 0; j < n; j++) {
        if (j >= 2 && isValidCutPoint(word, j)) {
            result.push_back('-');
        }
        result.push_back(word[j]);
    }
    return result;
}
