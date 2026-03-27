#include <algorithm>
#include <cctype>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
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

struct AVLNode {
  std::string key;
  std::unique_ptr<AVLNode> left;
  std::unique_ptr<AVLNode> right;
  int height;
  AVLNode(std::string k) : key(std::move(k)), height(1) {}
};

int height(const std::unique_ptr<AVLNode> &node) {
  return node ? node->height : 0;
}

int getBalance(const std::unique_ptr<AVLNode> &node) {
  return node ? height(node->left) - height(node->right) : 0;
}

std::unique_ptr<AVLNode> rightRotate(std::unique_ptr<AVLNode> y) {
  auto x = std::move(y->left);
  auto T2 = std::move(x->right);
  y->left = std::move(T2);
  y->height = std::max(height(y->left), height(y->right)) + 1;
  x->right = std::move(y);
  x->height = std::max(height(x->left), height(x->right)) + 1;
  return x;
}

std::unique_ptr<AVLNode> leftRotate(std::unique_ptr<AVLNode> x) {
  auto y = std::move(x->right);
  auto T2 = std::move(y->left);
  x->right = std::move(T2);
  x->height = std::max(height(x->left), height(x->right)) + 1;
  y->left = std::move(x);
  y->height = std::max(height(y->left), height(y->right)) + 1;
  return y;
}

std::unique_ptr<AVLNode> avlInsert(std::unique_ptr<AVLNode> node,
                                   std::string key) {
  if (!node)
    return std::make_unique<AVLNode>(std::move(key));

  if (key < node->key)
    node->left = avlInsert(std::move(node->left), key);
  else if (key > node->key)
    node->right = avlInsert(std::move(node->right), key);
  else
    return node;

  node->height = 1 + std::max(height(node->left), height(node->right));
  int balance = getBalance(node);

  if (balance > 1 && key < node->left->key)
    return rightRotate(std::move(node));
  if (balance < -1 && key > node->right->key)
    return leftRotate(std::move(node));
  if (balance > 1 && key > node->left->key) {
    node->left = leftRotate(std::move(node->left));
    return rightRotate(std::move(node));
  }
  if (balance < -1 && key < node->right->key) {
    node->right = rightRotate(std::move(node->right));
    return leftRotate(std::move(node));
  }

  return node;
}

void inorder(const AVLNode *node, std::vector<std::string> &sorted) {
  if (!node)
    return;
  inorder(node->left.get(), sorted);
  sorted.push_back(node->key);
  inorder(node->right.get(), sorted);
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
  std::cout << "Iniciando Arbol AVL...\n";
  std::unique_ptr<AVLNode> root = nullptr;

  auto t0 = std::chrono::high_resolution_clock::now();
  for (const auto &w : words) {
    root = avlInsert(std::move(root), w);
  }

  std::vector<std::string> sorted;
  sorted.reserve(words.size());
  inorder(root.get(), sorted);
  auto t1 = std::chrono::high_resolution_clock::now();

  double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

  saveResult("result_avl.txt", "AVL Tree", sorted, ms);

  std::cout << "\n[AVL Tree]\n";
  std::cout << "  Tiempo    : " << formatTime(ms) << "\n";
  std::cout << "  Ordenado  : " << (isSorted(sorted) ? "SI" : "NO") << "\n";
  std::cout << "  Peso input: " << std::fixed << std::setprecision(2)
            << getFileSizeMB(dataset) << " MB\n";
  std::cout << "  Peso out  : " << std::fixed << std::setprecision(2)
            << getFileSizeMB("result_avl.txt") << " MB\n";

  std::cout << "\n--- Complexity Analysis ---\n";
  std::cout << std::left << std::setw(12) << "Algorithm" << std::setw(16)
            << "Best" << std::setw(16) << "Average" << std::setw(16) << "Worst"
            << "Space\n";
  std::cout << std::string(76, '-') << "\n";
  std::cout << std::setw(12) << "AVL Tree" << std::setw(16) << "O(n log n)"
            << std::setw(16) << "O(n log n)" << std::setw(16) << "O(n log n)"
            << "O(n)\n";

  return 0;
}