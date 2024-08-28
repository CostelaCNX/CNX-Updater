#pragma once

#include <borealis.hpp>

class CreditsTab : public brls::List
{
public:
    CreditsTab();

    View* getDefaultFocus() override
    {
        return nullptr;
    }
};
