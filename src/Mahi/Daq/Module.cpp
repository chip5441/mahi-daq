#include <Mahi/Daq/Module.hpp>
#include <algorithm>

#include <Mahi/Util/Logging/Log.hpp>
using namespace mahi::util;

namespace mahi {
namespace daq {

//==============================================================================
// HELPER FUNCTIONS
//==============================================================================

namespace  {

ChanNums sort_and_reduce_channels(const ChanNums& channels) {
    ChanNums sorted_channels = channels;
    std::sort(sorted_channels.begin(), sorted_channels.end());
    sorted_channels.erase(std::unique(sorted_channels.begin(), sorted_channels.end()), sorted_channels.end());
    return sorted_channels;
}

ChanMap make_channel_map(const ChanNums& channel_numbers) {
    ChanMap channel_map;
    for (std::size_t i = 0; i < channel_numbers.size(); ++i)
        channel_map[channel_numbers[i]] = i;
    return channel_map;
}

}

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

ModuleBase::ModuleBase() { }

ModuleBase::ModuleBase(const ChanNums& channel_numbers) 
    : channel_numbers_(channel_numbers)
{
    update_map();
}

bool ModuleBase::on_enable() {
    // return true by default
    return true;
}

bool ModuleBase::on_disable() {
    // return true by default
    return true;
}

bool ModuleBase::update_channel(ChanNum channel) {
    // TODO
    return true;
}

bool ModuleBase::update() {
    bool success = true;
    for (auto& ch : channel_numbers_) 
        success = update_channel(ch) ? success : false;
    return success;
}

void ModuleBase::set_channel_numbers(const ChanNums& channel_numbers) {
    auto new_channels = sort_and_reduce_channels(channel_numbers);
    if (new_channels != channel_numbers_) {
        auto old_channels = channel_numbers_;
        channel_numbers_ = new_channels;
        update_map();
        on_channels_changed(old_channels, channel_numbers_);
        LOG(Verbose) << "Set Module " << get_name() << " channel numbers";// to " << channel_numbers_;
    }
}

void ModuleBase::add_channel_number(ChanNum channel_number) {
    if (!channel_map_.count(channel_number)) {
        auto old_channels = channel_numbers_;
        channel_numbers_.push_back(channel_number);
        sort_and_reduce_channels(channel_numbers_);
        update_map(); 
        on_channels_changed(old_channels, channel_numbers_);
        LOG(Verbose) << "Added channel number " << channel_number << " to Module " << get_name();      
    } 
}

void ModuleBase::remove_channel_number(ChanNum channel_number) {
    if (channel_map_.count(channel_number)) {
        auto old_channels = channel_numbers_;
        channel_numbers_.erase(std::remove(channel_numbers_.begin(), channel_numbers_.end(), channel_number), channel_numbers_.end());
        update_map();
        on_channels_changed(old_channels, channel_numbers_);
        LOG(Verbose) << "Removed channel number " << channel_number << " from Module " << get_name();      
    }
}

void ModuleBase::on_channels_changed(const ChanNums& old_channels, const ChanNums& new_channels) {
    // do nothing by default
}

void ModuleBase::update_map() {
    ChanMap old_map = channel_map_;
    channel_map_ = make_channel_map(channel_numbers_);
    for (std::size_t i = 0; i < buffers_.size(); i++)
        buffers_[i]->change_channel_numbers(old_map, channel_map_);    
}

const ChanNums& ModuleBase::channel_numbers() const {
    return channel_numbers_;
}

std::size_t ModuleBase::channel_count() const {
    return channel_numbers_.size();
}

bool ModuleBase::validate_channel_number(ChanNum channel_number, bool quiet) const {
    if (channel_map_.count(channel_number) > 0)
        return true;
    if (!quiet) {
        LOG(Error) << "Invalid channel number " << channel_number << " not declared in channel numbers on Module " << get_name();
    }
    return false;
}

bool ModuleBase::validate_channel_count(std::size_t size, bool quiet) const {
    if (channel_numbers_.size() == size)
        return true;
    if (!quiet) {
        LOG(Error) << "Invalid number of elements (" << size << ") not equal to channel count of " << channel_count() << " on Module " << get_name();
    }
    return false;
}

void ModuleBase::add_buffer(ModuleArrayBase* buffer) {
    buffers_.push_back(buffer);
}

std::size_t ModuleBase::index(ChanNum channel_number) const {
    return channel_map_.at(channel_number);
}

} // namespace daq
} // namespace mahi
