#include <iostream>
#include <map>
using namespace std;
static int user_id;

class Rack {
public:
  int id;

  Rack(int _id) { id = _id; }
};

class Book {
public:
  string isbn;
  string name;
  // Author author;
  string author_name;
  int price;

  Book(string _name, string _isbn, string _author_name, int _price) {
    name = _name;
    isbn = _isbn;
    author_name = _author_name;
    // copies_available = _copies_available;
    price = _price;
  }

  Book(string code) {}
};

class BookItem : public Book {
public:
  bool is_available;
  string barcode;

  BookItem(string _name, string _isbn, string _author_name, int _price,
           string _barcode)
      : Book(_name, _isbn, _author_name, _price) {
    is_available = true;
    // copies_available = copies;
    barcode = _barcode;
  }

  void pickBookItem() { is_available = false; }

  void putBackBookItem() { is_available = true; }
};

class Library {
public:
  map<string, Book *> books;
  map<string, BookItem *> bookitems;

  Library() {
    auto x = new Book("book1", "isbn1", "manosriram", 1000);
    books[x->isbn] = x;

    for (int t = 0; t < 3; ++t) {
      auto y = new BookItem("barcode" + std::to_string(t));
      bookitems[y->barcode] = y;
    }
  }

  int showAvailableBookItemsCount(Book *book) {
    int ct = 0;
    for (auto x : bookitems) {
      if (x.second->isbn == book->isbn) {
        ct++;
      }
    }
    return ct;
  }
};

class User {
protected:
  string name;
  int id;

  User() {}
};

class Member : public User {
public:
  int pending_fee;
  map<string, Book *> user_books;
  Member(string username) {
    id = user_id++;
    pending_fee = 0;
    name = username;
  }
};

class Admin : public User, public Library {
public:
  Admin(string username) {
    id = user_id++;
    name = username;
  }

  bool isBookAvailable(BookItem *book) {
    for (auto x : bookitems) {
      if (x.second->isbn == book->isbn && x.second->is_available) {
        return true;
        // return x.second;
      }
    }
    return false;
  }

  // void calculateFee() {}
  void checkInBook(Member *m, BookItem *book) {
    bool is_available = isBookAvailable(book);
    if (!is_available) {
      cout << "book out of stock" << endl;
      return;
    }
    m->pending_fee += book->price;
    m->user_books[book->isbn] = book;

    book->pickBookItem();
  }
  void checkOutBook(Member *m, BookItem *book) {
    m->pending_fee -= book->price;
    delete m->user_books[book->isbn];

    book->putBackBookItem();
  }
};

int main() {
  auto l = new Library();

  auto mem = new Member("manosriram");
  auto ad = new Admin("mano");
  ad->checkInBook(mem, l->bookitems["barcode1"]);
}
