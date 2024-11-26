#pragma once
#include <cmath>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

#include "common/assert.hpp"
#include "common/types.hpp"
#include "common/utils.hpp"
#include "fakeMarket/clientUpdater.hpp"
#include "fakeMarket/events.hpp"

namespace FakeMarket {

namespace Test {
class OrderBookTestFixture;
}

class OrderBook {
   public:
    using SingleQuoteDetails = std::tuple<Side, Price, Volume>;
    using QuoteInfo = std::pair<Volume, Common::HashId>;
    using QuoteVec = std::vector<QuoteInfo>;
    using BidQuotesMap = std::map<Price, QuoteVec, std::greater<Price>>;
    using AskQuotesMap = std::map<Price, QuoteVec>;

    OrderBook(ClientUpdater& clientUpdater, double tickSize)
        : mClientUpdater{clientUpdater}, mTickSize{tickSize} {}

    bool QuoteDelete(const SubmitQuoteDelete&);

    bool QuoteUpdate(const SubmitQuoteUpdate&);

    bool FAK(const SubmitFAK&);

    TOB GetTopOfBook() const;

    void SetBook(BidQuotesMap, AskQuotesMap);

   private:
    template <typename Compare>
    Volume CrossBook(Price, Volume, std::map<Price, QuoteVec, Compare>&);

    bool CheckValidPrice(Price price) {
        return std::abs(price - std::round(price / mTickSize) * mTickSize) <
               1e-9;
    }

    QuoteVec& GetPriceLevel(Side side, Price price) {
        if (side == Side::BUY) {
            auto it = mBidQuotes.find(price);
            ASSERT(it != mBidQuotes.end());
            return it->second;
        } else {
            auto it = mAskQuotes.find(price);
            ASSERT(it != mAskQuotes.end());
            return it->second;
        }
    }

    ClientUpdater& mClientUpdater;
    double mTickSize;

    std::unordered_map<Common::HashId, SingleQuoteDetails> mQuotes;
    BidQuotesMap mBidQuotes;
    AskQuotesMap mAskQuotes;

    friend class Test::OrderBookTestFixture;
};

}  // namespace FakeMarket
