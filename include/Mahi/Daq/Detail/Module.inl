#ifdef _MSC_VER
#pragma warning( disable : 4589 )
#endif

namespace mahi {
namespace daq {

template <typename T>
Module<T>::Module() :
    ModuleBase(),
    values_(this),
    min_values_(this),
    max_values_(this)
{}

template <typename T>
Module<T>::Module(const ChanNums& channel_numbers) :
    ModuleBase(channel_numbers),
    values_(this),
    min_values_(this),
    max_values_(this)
{}

template <typename T>
Module<T>::~Module() { }

template <typename T>
const T& Module<T>::operator[](ChanNum channel_number) const {
    return values_[channel_number];
}

template <typename T>
T& Module<T>::operator[](ChanNum channel_number) {
    return values_[channel_number];
}

template <typename T>
bool Module<T>::set_ranges(const std::vector<T>& min_values, const std::vector<T>& max_values) {
    min_values_.set_raw(min_values);
    max_values_.set_raw(max_values);
    return true;
}

template <typename T>
bool Module<T>::set_range(ChanNum channel_number, T min_value, T max_value) {
    if (validate_channel_number(channel_number)) {
        min_values_[channel_number] = min_value;
        max_values_[channel_number] = max_value;
        return true;
    }
    return false;
}

template <typename T>
std::vector<T>& Module<T>::get() {
    return values_.get_raw();
}

template <typename T>
const T& Module<T>::get(ChanNum channel_number) const {
    return values_[channel_number];
}

template <typename T>
T& Module<T>::get(ChanNum channel_number) {
    return values_[channel_number];
}

template <typename T>
void Module<T>::set(const std::vector<T>& values) {
    values_.set_raw(values);
}

template <typename T>
void Module<T>::set(ChanNum channel_number, T value) {
    values_[channel_number] = value;
}

} // namespace daq
} // namespace mahi
