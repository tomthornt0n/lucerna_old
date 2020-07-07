#define LC_MESSAGE_TYPE_COUNT 7

typedef enum
{
    LC_MESSAGE_TYPE_WINDOW_CLOSE,
    LC_MESSAGE_TYPE_WINDOW_RESIZE,

    LC_MESSAGE_TYPE_KEY_PRESS,
    LC_MESSAGE_TYPE_KEY_RELEASE,

    LC_MESSAGE_TYPE_MOUSE_BUTTON_PRESS,
    LC_MESSAGE_TYPE_MOUSE_BUTTON_RELEASE,
    LC_MESSAGE_TYPE_MOUSE_SCROLL
} lcMessageType_t;

typedef struct lcMessage_t
{
    lcMessageType_t Type;

    union
    {
        struct lcMessageWindowResize_t
        {
            uint32_t Width, Height;
        } WindowResize;

        struct lcMessageKeyPress_t
        {
            int KeyCode;
            uint8_t Repeat;
        } KeyPress;

        struct lcMessageKeyRelease_t
        {
            int KeyCode;
        } KeyRelease;

        struct lcMessageMouseButtonPress_t
        {
            int KeyCode;
        } MouseButtonPress;

        struct lcMessageMouseButtonRelease_t
        {
            int KeyCode;
        } MouseButtonRelease;

        struct lcMessageMouseScroll_t
        {
            int XOffset, YOffset;
        } MouseScroll;
    };
} lcMessage_t;

typedef void (*lcMessageListener_t)(lcMessage_t);

lcMessageListener_t *_lc_MessageListeners[LC_MESSAGE_TYPE_COUNT];

lc_MessageSystemInit(void)
{
    int i;
    for (i = 0; i < LC_MESSAGE_TYPE_COUNT; ++i)
    {
        _lc_MessageListeners[i] = NULL; 
        LC_LIST_CREATE(_lc_MessageListeners[i], lcMessageListener_t); 
    }
}

lcMessage_t lc_WindowCloseMessageCreate(void)
{
    lcMessage_t message;
    message.Type = LC_MESSAGE_TYPE_WINDOW_CLOSE;

    return message;
}

lcMessage_t lc_WindowResizeMessageCreate(uint32_t width,
                                         uint32_t height)
{
    lcMessage_t message;
    message.Type = LC_MESSAGE_TYPE_WINDOW_RESIZE;

    message.WindowResize.Width = width;
    message.WindowResize.Height = height;

    return message;
}

lcMessage_t lc_KeyPressMessageCreate(int keyCode,
                                     uint8_t repeat)
{
    lcMessage_t message;
    message.Type = LC_MESSAGE_TYPE_KEY_PRESS;

    message.KeyPress.KeyCode = keyCode;
    message.KeyPress.Repeat = repeat;

    return message;
}

lcMessage_t lc_KeyReleaseMessageCreate(int keyCode)
{
    lcMessage_t message;
    message.Type = LC_MESSAGE_TYPE_KEY_RELEASE;

    message.KeyRelease.KeyCode = keyCode;

    return message;
}

lcMessage_t lc_MouseButtonPressMessageCreate(int keyCode)
{
    lcMessage_t message;
    message.Type = LC_MESSAGE_TYPE_MOUSE_BUTTON_PRESS;

    message.MouseButtonPress.KeyCode = keyCode;

    return message;
}

lcMessage_t lc_MouseButtonReleaseMessageCreate(int keyCode)
{
    lcMessage_t message;
    message.Type = LC_MESSAGE_TYPE_MOUSE_BUTTON_RELEASE;

    message.MouseButtonRelease.KeyCode = keyCode;

    return message;
}

lcMessage_t lc_MouseScrollMessageCreate(int xOffset,
                                         int yOffset)
{
    lcMessage_t message;
    message.Type = LC_MESSAGE_TYPE_MOUSE_SCROLL;

    message.MouseScroll.XOffset = xOffset;
    message.MouseScroll.YOffset = yOffset;

    return message;
}

lc_MessageBind(lcMessageType_t messageType,
               lcMessageListener_t message)
{
    LC_LIST_PUSH_BACK(_lc_MessageListeners[messageType],
                      lcMessageListener_t,
                      &message);
}

lc_MessageEmit(lcMessage_t message)
{
    int i;
    for (i = 0; i < LC_LIST_LEN(_lc_MessageListeners[message.Type]); ++i)
    {
        (*_lc_MessageListeners[message.Type][i])(message);
    }
}

lc_MessageSystemDestroy()
{
    int i;
    for (i = 0; i < LC_MESSAGE_TYPE_COUNT; ++i)
    {
        LC_LIST_DESTROY(_lc_MessageListeners[i]);
    }
}
