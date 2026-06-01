#ifndef MIDI_PARSERSYNTHESIZER_LOCKFREEQUEUE_H
#define MIDI_PARSERSYNTHESIZER_LOCKFREEQUEUE_H
#include <atomic>

template <typename T, size_t N>
class LockFreeQueue {
private:
    std::array<T, N> buffer;
    std::atomic<size_t> read_idx;
    std::atomic<size_t> write_idx;

public:
    /**
     * Adds the item parameter to <tt>this</tt>. If the queue is full, the item is not added.
     * @param item The item to be added to <tt>this</tt>, which will not be modified.
     * @return A boolean value that is @c true if the item was successfully added to <tt>this</tt>,
     * or @c false if the item was not added to <tt>this</tt>.
     */
    bool push(const T& item) {
        const auto current_write = write_idx.load(std::memory_order_relaxed);
        const auto next_write = (current_write + 1 >= N) ? 0 : current_write + 1;

        if (next_write == read_idx.load(std::memory_order_acquire)) {
            return false;
        }

        buffer[current_write] = item;
        write_idx.store(next_write, std::memory_order_release);
        return true;
    }

    /**
     * Removes the next item from <tt>this</tt>, and returns it in the item output parameter.
     * If there is no next item in the buffer, the output parameter is not modified.
     * @param item The item parameter which will contain the output of this method.
     * Modified if there is a next item, and not modified is there is no next item.
     * @return A boolean value that is @c true if there was a next item in <tt>this</tt>,
     * or @c false if there was no next item in <tt>this</tt>.
     */
    bool pop(T& item) {
        const auto current_read = read_idx.load(std::memory_order_relaxed);

        if (current_read == write_idx.load(std::memory_order_acquire)) {
            return false;
        }

        item = buffer[current_read];
        read_idx.store((current_read + 1 >= N) ? 0 : current_read + 1, std::memory_order_release);
        return true;
    }

    bool peek(T& item) {
        const auto current_read = read_idx.load(std::memory_order_relaxed);

        if (current_read == write_idx.load(std::memory_order_acquire)) {
            return false;
        }

        item = buffer[current_read];
        return true;
    }
};

#endif //MIDI_PARSERSYNTHESIZER_LOCKFREEQUEUE_H