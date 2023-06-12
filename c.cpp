template <typename T>
class base {

protected:
    int x;
};

template <typename T>
class derived : public base<T> {

public:
    int f() { return this->x; }
};

int main() {
    derived<int> d;
    d.f();
    return 0;
}