#pragma once
#include <vector>

template <typename T>
class ObjectManager {
public:
    void add(T* object) {
        m_objects.push_back(std::shared_ptr<T>(object));
    }

    void add(std::shared_ptr<T> object) {
        m_objects.push_back(object);
    }

    void remove(T* object) {
        for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
            if (it->get() == object) {
                m_objects.erase(it);
                return;
            }
        }
    }

    void clear() {
        m_objects.clear();
    }

    size_t size() const {
        return m_objects.size();
    }

    std::shared_ptr<T> get(size_t index) {
        if (index < m_objects.size()) {
            return m_objects[index];
        }
        return nullptr;
    }

private:
    std::vector<std::shared_ptr<T>> m_objects;
};
