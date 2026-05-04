#include "TCA8418KeyboardBase.h"

/**
 * @brief 6x6 keyboard based on blackberry keyboard with 5 position tophat switch
 */
class Thumby : public TCA8418KeyboardBase
{
  public:
    Thumby();
    void reset(void) override;

  protected:
    void pressed(uint8_t key) override;
    void released(void) override;

    bool shift_pending  = false;
    bool alt_pending    = false;
    uint8_t last_key;
};
