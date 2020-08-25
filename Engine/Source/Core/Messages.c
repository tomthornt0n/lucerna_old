/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 24 August 2020
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

static lcGenericMessage_t *
lcWindowCloseMessageCreate(void)
{
    lcGenericMessage_t *message = malloc(sizeof(lcGenericMessage_t));
    message->Type = LC_MESSAGE_TYPE_WINDOW_CLOSE;

    return message;
}

static lcGenericMessage_t *
lcWindowResizeMessageCreate(uint32_t width,
                            uint32_t height)
{
    lcWindowResizeMessage_t *message = malloc(sizeof(lcWindowResizeMessage_t));

    message->Header.Type = LC_MESSAGE_TYPE_WINDOW_RESIZE;
    message->Width = width;
    message->Height = height;

    return (lcGenericMessage_t *)message;
}

static lcGenericMessage_t *
lcKeyPressMessageCreate(int keyCode)
{
    lcKeyPressMessage_t *message = malloc(sizeof(lcKeyPressMessage_t));

    message->Header.Type = LC_MESSAGE_TYPE_KEY_PRESS;
    message->KeyCode = keyCode;

    return (lcGenericMessage_t *)message;
}

static lcGenericMessage_t *
lcKeyReleaseMessageCreate(int keyCode)
{
    lcKeyReleaseMessage_t *message = malloc(sizeof(lcKeyReleaseMessage_t));

    message->Header.Type = LC_MESSAGE_TYPE_KEY_RELEASE;
    message->KeyCode = keyCode;

    return (lcGenericMessage_t *)message;
}

static lcGenericMessage_t *
lcMouseButtonPressMessageCreate(int keyCode)
{
    lcMouseButtonPressMessage_t *message =
        malloc(sizeof(lcMouseButtonPressMessage_t));

    message->Header.Type = LC_MESSAGE_TYPE_MOUSE_BUTTON_PRESS;
    message->KeyCode = keyCode;

    return (lcGenericMessage_t *)message;
}

static lcGenericMessage_t *
lcMouseButtonReleaseMessageCreate(int keyCode)
{
    lcMouseButtonReleaseMessage_t *message =
        malloc(sizeof(lcMouseButtonReleaseMessage_t));

    message->Header.Type = LC_MESSAGE_TYPE_MOUSE_BUTTON_RELEASE;
    message->KeyCode = keyCode;

    return (lcGenericMessage_t *)message;
}

static lcGenericMessage_t *
lcMouseScrollMessageCreate(int offset)
{
    lcMouseScrollMessage_t *message =
        malloc(sizeof(lcMouseScrollMessage_t));

    message->Header.Type = LC_MESSAGE_TYPE_MOUSE_SCROLL;
    message->ScrollOffset = offset;

    return (lcGenericMessage_t *)message;
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
lcMessageEmit(lcGenericMessage_t *message)
{
    int i;
    for (i = 0; i < LC_LIST_LEN(lcMessageListeners[message->Type]); ++i)
    {
        (*lcMessageListeners[message->Type][i])(message);
    }
    free(message);
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

