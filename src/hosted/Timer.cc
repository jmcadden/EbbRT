//          Copyright Boston University SESA Group 2013 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
#include "../Timer.h"
#include "Clock.h"
#include "Context.h"
#include "EventManager.h"

const constexpr ebbrt::EbbId ebbrt::Timer::static_id;
std::shared_ptr<boost::asio::deadline_timer> t;

ebbrt::Timer::Timer() {
  t = std::make_shared<boost::asio::deadline_timer>(
      active_context->io_service_);
}

void ebbrt::Timer::SetTimer(std::chrono::microseconds from_now) {
  // Any pending asynchronous wait operations will be cancelled
  t->expires_from_now(boost::posix_time::microseconds(from_now.count()));
  t->async_wait(EventManager::WrapHandler(
      [this](const boost::system::error_code& e) {
        if (e != boost::asio::error::operation_aborted) {
          // Timer was not cancelled, take necessary action.
          auto now = ebbrt::clock::Wall::Now().time_since_epoch();
          while (!timers_.empty() && timers_.begin()->fire_time_ <= now) {
            auto& hook = *timers_.begin();

            // remove from the set
            timers_.erase(hook);

            // If it needs repeating, put it back in with the updated time
            if (hook.repeat_us_ != std::chrono::microseconds::zero()) {
              hook.fire_time_ = now + hook.repeat_us_;
              timers_.insert(hook);
            }

            hook.Fire();
            now = clock::Wall::Now().time_since_epoch();
          }
          if (!timers_.empty()) {
            SetTimer(std::chrono::duration_cast<std::chrono::microseconds>(
                timers_.begin()->fire_time_ - now));
          }
        }
  }));
}

void ebbrt::Timer::StopTimer() {
  t->cancel();
}

void ebbrt::Timer::Start(Hook& hook, std::chrono::microseconds timeout,
                         bool repeat) {

  auto now = clock::Wall::Now().time_since_epoch();
  hook.fire_time_ = now + timeout;
  if (timers_.empty() || *timers_.begin() > hook) {
    SetTimer(timeout); 
  }
  hook.repeat_us_ = repeat ? timeout : std::chrono::microseconds::zero();
  timers_.insert(hook);
}

void ebbrt::Timer::Stop(Hook& hook) { 
  // TODO: check if hook is at begining of timers_ list to avoid an
  // unnessessary timeout
  timers_.erase(hook);
  if (timers_.empty()) {
    StopTimer();
  }   
}
