/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 25 July 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

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

void
lc_MessageSystemInit(void)
{
    int i;
    for (i = 0; i < LC_MESSAGE_TYPE_COUNT; ++i)
    {
        _lc_MessageListeners[i] = NULL; 
        LC_LIST_CREATE(_lc_MessageListeners[i], lcMessageListener_t); 
    }
}

lcMessage_t
lc_WindowCloseMessageCreate(void)
{
    lcMessage_t message;
    message.Type = LC_MESSAGE_TYPE_WINDOW_CLOSE;

    return message;
}

lcMessage_t
lc_WindowResizeMessageCreate(uint32_t width,
                             uint32_t height)
{
    lcMessage_t message;
    message.Type = LC_MESSAGE_TYPE_WINDOW_RESIZE;

    message.WindowResize.Width = width;
    message.WindowResize.Height = height;

    return message;
}

lcMessage_t
lc_KeyPressMessageCreate(int keyCode,
                         uint8_t repeat)
{
    lcMessage_t message;
    message.Type = LC_MESSAGE_TYPE_KEY_PRESS;

    message.KeyPress.KeyCode = keyCode;
    message.KeyPress.Repeat = repeat;

    return message;
}

lcMessage_t
lc_KeyReleaseMessageCreate(int keyCode)
{
    lcMessage_t message;
    message.Type = LC_MESSAGE_TYPE_KEY_RELEASE;

    message.KeyRelease.KeyCode = keyCode;

    return message;
}

lcMessage_t
lc_MouseButtonPressMessageCreate(int keyCode)
{
    lcMessage_t message;
    message.Type = LC_MESSAGE_TYPE_MOUSE_BUTTON_PRESS;

    message.MouseButtonPress.KeyCode = keyCode;

    return message;
}

lcMessage_t
lc_MouseButtonReleaseMessageCreate(int keyCode)
{
    lcMessage_t message;
    message.Type = LC_MESSAGE_TYPE_MOUSE_BUTTON_RELEASE;

    message.MouseButtonRelease.KeyCode = keyCode;

    return message;
}

lcMessage_t
lc_MouseScrollMessageCreate(int xOffset,
                            int yOffset)
{
    lcMessage_t message;
    message.Type = LC_MESSAGE_TYPE_MOUSE_SCROLL;

    message.MouseScroll.XOffset = xOffset;
    message.MouseScroll.YOffset = yOffset;

    return message;
}

void
lc_MessageBind(lcMessageType_t messageType,
               lcMessageListener_t message)
{
    LC_LIST_PUSH_BACK(_lc_MessageListeners[messageType],
                      lcMessageListener_t,
                      &message);
}

void
lc_MessageEmit(lcMessage_t message)
{
    int i;
    for (i = 0; i < LC_LIST_LEN(_lc_MessageListeners[message.Type]); ++i)
    {
        (*_lc_MessageListeners[message.Type][i])(message);
    }
}

void
lc_MessageSystemDestroy(void)
{
    int i;
    for (i = 0; i < LC_MESSAGE_TYPE_COUNT; ++i)
    {
        LC_LIST_DESTROY(_lc_MessageListeners[i]);
    }
}


/*
MIT License

Copyright (c) 2020 Tom Thornton

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

