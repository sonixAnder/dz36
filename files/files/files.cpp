#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;
namespace fs = std::filesystem;

struct SearchResult
{
    string file_path;
    int occurrences;
};

void search_in_file(const string& file_path, const string& word, vector<SearchResult>& results, bool replace = false, const string& replace_word = "")
{
    ifstream file(file_path);
    if (!file.is_open())
    {
        cerr << "Не могу открыть файл: " << file_path << endl;
        return;
    }

    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    size_t pos = 0;
    int count = 0;
    while ((pos = content.find(word, pos)) != string::npos)
    {
        ++count;
        pos += word.length();
    }

    if (count > 0)
    {
        results.push_back({ file_path, count });

        if (replace)
        {
            pos = 0;
            while ((pos = content.find(word, pos)) != string::npos)
            {
                content.replace(pos, word.length(), replace_word);
                pos += replace_word.length();
            }
            ofstream outfile(file_path);
            outfile << content;
            outfile.close();
        }
    }
}

void search_in_directory(const fs::path& directory, const string& word, vector<SearchResult>& results, bool replace = false, const std::string& replace_word = "")
{
    for (const auto& entry : fs::recursive_directory_iterator(directory))
    {
        if (entry.is_regular_file())
        {
            search_in_file(entry.path().string(), word, results, replace, replace_word);
        }
    }
}

void generate_report(const vector<SearchResult>& results, const string& report_path)
{
    ofstream report(report_path);
    if (!report.is_open())
    {
        cerr << "Не могу открыть файл отчета: " << report_path << endl;
        return;
    }

    for (const auto& result : results)
    {
        report << "Файл: " << result.file_path << ", События: " << result.occurrences << endl;
    }
    report.close();
}

int main()
{
    setlocale(LC_ALL, "ru");
    string directory, word, replace_word;
    char choice;
    bool replace = false;

    cout << "Введите путь к каталогу: ";
    cin >> directory;

    cout << "Введите слово для поиска: ";
    cin >> word;

    cout << "Хотите заменить слово? (y/n): ";
    cin >> choice;

    if (choice == 'y' || choice == 'Y')
    {
        replace = true;
        cout << "Введите слово, на которое нужно заменить: ";
        cin >> replace_word;
    }

    vector<SearchResult> results;
    search_in_directory(directory, word, results, replace, replace_word);

    string report_path = "search_report.txt";
    generate_report(results, report_path);

    cout << "Поиск завершен. Отчет создан в: " << report_path << endl;

    return 0;
}
