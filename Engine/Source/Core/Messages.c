/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 30 July 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static lcMessageListener_t *lcMessageListeners[LC_MESSAGE_TYPE_COUNT];

static void
lcMessageSystemInit(void)
{
    int i;
    for (i = 0; i < LC_MESSAGE_TYPE_COUNT; ++i)
    {
        lcMessageListeners[i] = NULL; 
        LC_LIST_CREATE(lcMessageListeners[i], lcMessageListener_t); 
    }
}

static lcMessage_t
lcWindowCloseMessageCreate(void)
{
    lcMessage_t message;
    message.Type = LC_MESSAGE_TYPE_WINDOW_CLOSE;

    return message;
}

static lcMessage_t
lcWindowResizeMessageCreate(uint32_t width,
                            uint32_t height)
{
    lcMessage_t message;
    message.Type = LC_MESSAGE_TYPE_WINDOW_RESIZE;

    message.WindowResize.Width = width;
    message.WindowResize.Height = height;

    return message;
}

static lcMessage_t
lcKeyPressMessageCreate(int keyCode,
                        bool repeat)
{
    lcMessage_t message;
    message.Type = LC_MESSAGE_TYPE_KEY_PRESS;

    message.KeyPress.KeyCode = keyCode;
    message.KeyPress.Repeat = repeat;

    return message;
}

static lcMessage_t
lcKeyReleaseMessageCreate(int keyCode)
{
    lcMessage_t message;
    message.Type = LC_MESSAGE_TYPE_KEY_RELEASE;

    message.KeyRelease.KeyCode = keyCode;

    return message;
}

static lcMessage_t
lcMouseButtonPressMessageCreate(int keyCode)
{
    lcMessage_t message;
    message.Type = LC_MESSAGE_TYPE_MOUSE_BUTTON_PRESS;

    message.MouseButtonPress.KeyCode = keyCode;

    return message;
}

static lcMessage_t
lcMouseButtonReleaseMessageCreate(int keyCode)
{
    lcMessage_t message;
    message.Type = LC_MESSAGE_TYPE_MOUSE_BUTTON_RELEASE;

    message.MouseButtonRelease.KeyCode = keyCode;

    return message;
}

static lcMessage_t
lcMouseScrollMessageCreate(int xOffset,
                           int yOffset)
{
    lcMessage_t message;
    message.Type = LC_MESSAGE_TYPE_MOUSE_SCROLL;

    message.MouseScroll.XOffset = xOffset;
    message.MouseScroll.YOffset = yOffset;

    return message;
}

void
lcMessageBind(int messageType,
              lcMessageListener_t action)
{
    LC_LIST_PUSH_BACK(lcMessageListeners[messageType],
                      lcMessageListener_t,
                      &action);
}

static void
lcMessageEmit(lcMessage_t message)
{
    int i;
    for (i = 0; i < LC_LIST_LEN(lcMessageListeners[message.Type]); ++i)
    {
        (*lcMessageListeners[message.Type][i])(message);
    }
}

static void
lcMessageSystemDestroy(void)
{
    int i;
    for (i = 0; i < LC_MESSAGE_TYPE_COUNT; ++i)
    {
        LC_LIST_DESTROY(lcMessageListeners[i]);
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

