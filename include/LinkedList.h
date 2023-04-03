#pragma once

namespace TRM {

template <typename T>
struct LinkedList {
  T* head = nullptr;
  T* tail = nullptr;

  LinkedList() = default;
  LinkedList(T* head, T* tail): head(head), tail(tail) {}


  template <T* T::*Prev, T* T::*Next>
  static void Insert(T* t, T* prev, T* next, T** head, T** tail) {
    t->*Prev = prev;
    t->*Next = next;

    if (prev) {
      prev->*Next = t;
    } else if (head) {
      *head = t;
    }

    if (next) {
      next->*Prev = t;
    } else if (tail) {
      *tail = t;
    }
  }

  template <T* T::*Prev, T* T::*Next>
  void Remove(T* t, T** head, T** tail) {
    if (t->*Prev) {
      t->*Prev->*Next = t->*Next;
    } else if (head) {
      *head = t->*Next;
    }

    if (t->*Next) {
      t->*Next->*Prev = t->*Prev;
    } else if (tail) {
      *tail = t->*Prev;
    }

    t->*Prev = t->*Next = nullptr;
  }
};

}