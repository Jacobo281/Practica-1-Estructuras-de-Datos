#include <algorithm>
#include <cctype>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <vector>

double getFileSizeMB(const std::string &filename) {
  struct stat stat_buf;
  int rc = stat(filename.c_str(), &stat_buf);
  return rc == 0 ? stat_buf.st_size / (1024.0 * 1024.0) : -1.0;
}

std::string formatTime(double total_ms) {
  long long total_ms_int = static_cast<long long>(total_ms);
  long long mins = total_ms_int / 60000;
  long long secs = (total_ms_int % 60000) / 1000;
  double rem_ms = total_ms - (mins * 60000) - (secs * 1000);

  std::ostringstream oss;
  if (mins > 0)
    oss << mins << " min, " << secs << " s, " << std::fixed
        << std::setprecision(4) << rem_ms << " ms";
  else if (secs > 0)
    oss << secs << " s, " << std::fixed << std::setprecision(4) << rem_ms
        << " ms";
  else
    oss << std::fixed << std::setprecision(4) << rem_ms << " ms";
  return oss.str();
}

std::vector<std::string> loadWords(const std::string &filename) {
  std::vector<std::string> words;
  std::ifstream file(filename);
  std::string word;
  if (file.is_open()) {
    while (file >> word) {
      word.erase(std::remove_if(word.begin(), word.end(),
                                [](char c) {
                                  return !std::isalpha(
                                      static_cast<unsigned char>(c));
                                }),
                 word.end());
      if (!word.empty())
        words.push_back(word);
    }
  } else {
    std::cerr << "ERROR FATAL: No se pudo abrir " << filename << "\n";
  }
  return words;
}

int partition(std::vector<std::string> &arr, int low, int high) {
  int mid = low + (high - low) / 2;
  std::swap(arr[mid], arr[high]);
  std::string pivot = arr[high];

  int i = (low - 1);
  for (int j = low; j <= high - 1; j++) {
    if (arr[j] < pivot) {
      i++;
      std::swap(arr[i], arr[j]);
    }
  }
  std::swap(arr[i + 1], arr[high]);
  return (i + 1);
}

void quickSort(std::vector<std::string> &arr, int low, int high) {
  while (low < high) {
    int pi = partition(arr, low, high);
    if (pi - low < high - pi) {
      quickSort(arr, low, pi - 1);
      low = pi + 1;
    } else {
      quickSort(arr, pi + 1, high);
      high = pi - 1;
    }
  }
}

bool isSorted(const std::vector<std::string> &arr) {
  for (size_t i = 1; i < arr.size(); ++i) {
    if (arr[i - 1] > arr[i])
      return false;
  }
  return true;
}

void saveResult(const std::string &filename, const std::string &algoName,
                const std::vector<std::string> &sorted, double ms) {
  std::ofstream out(filename);
  if (out.is_open()) {
    out << algoName << " tardo: " << formatTime(ms) << "\n\n";
    for (const auto &w : sorted)
      out << w << "\n";
  }
}

int main() {
  std::string dataset = "words_alpha.txt";
  auto words = loadWords(dataset);

  if (words.empty())
    return 1;

  std::cout << "Se cargaron " << words.size() << " palabras limpias.\n";
  std::cout << "Iniciando QuickSort...\n";

  auto t0 = std::chrono::high_resolution_clock::now();
  quickSort(words, 0, words.size() - 1);
  auto t1 = std::chrono::high_resolution_clock::now();

  double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

  saveResult("result_quicksort.txt", "QuickSort", words, ms);

  std::cout << "\n[QuickSort]\n";
  std::cout << "  Tiempo    : " << formatTime(ms) << "\n";
  std::cout << "  Ordenado  : " << (isSorted(words) ? "SI" : "NO") << "\n";
  std::cout << "  Peso input: " << std::fixed << std::setprecision(2)
            << getFileSizeMB(dataset) << " MB\n";
  std::cout << "  Peso out  : " << std::fixed << std::setprecision(2)
            << getFileSizeMB("result_quicksort.txt") << " MB\n";

  std::cout << "\n--- Complexity Analysis ---\n";
  std::cout << std::left << std::setw(12) << "Algorithm" << std::setw(16)
            << "Best" << std::setw(16) << "Average" << std::setw(16) << "Worst"
            << "Space\n";
  std::cout << std::string(76, '-') << "\n";
  std::cout << std::setw(12) << "QuickSort" << std::setw(16) << "O(n log n)"
            << std::setw(16) << "O(n log n)" << std::setw(16) << "O(n^2)"
            << "O(log n)\n";

  return 0;
}