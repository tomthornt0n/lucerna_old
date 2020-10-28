/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Lucerna
  
  Author  : Tom Thornton
  Updated : 28 Oct 2020
  License : MIT, at end of file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

internal lcMessageHandler_t *_lcMessageHandlers[LC_MESSAGE_TYPE_COUNT];

internal void
_lcMessageSystemInit(void)
{
    i32 i;
    for (i = 0; i < LC_MESSAGE_TYPE_COUNT; ++i)
    {
        _lcMessageHandlers[i] = NULL; 
        LC_LIST_CREATE(_lcMessageHandlers[i]); 
    }
}

internal lcGenericMessage_t *
_lcWindowCloseMessageCreate(void)
{
    lcGenericMessage_t *message = malloc(sizeof(lcGenericMessage_t));
    message->Type = LC_MESSAGE_TYPE_WINDOW_CLOSE;

    return message;
}

internal lcGenericMessage_t *
_lcWindowResizeMessageCreate(u32 width,
                             u32 height)
{
    lcWindowResizeMessage_t *message;

    message = malloc(sizeof(lcWindowResizeMessage_t));
    message->Header.Type = LC_MESSAGE_TYPE_WINDOW_RESIZE;
    message->Width = width;
    message->Height = height;

    return (lcGenericMessage_t *)message;
}

internal lcGenericMessage_t *
_lcKeyPressMessageCreate(i32 keyCode)
{
    lcKeyPressMessage_t *message;

    message = malloc(sizeof(lcKeyPressMessage_t));
    message->Header.Type = LC_MESSAGE_TYPE_KEY_PRESS;
    message->KeyCode = keyCode;

    return (lcGenericMessage_t *)message;
}

internal lcGenericMessage_t *
_lcKeyReleaseMessageCreate(i32 keyCode)
{
    lcKeyReleaseMessage_t *message;

    message = malloc(sizeof(lcKeyReleaseMessage_t));
    message->Header.Type = LC_MESSAGE_TYPE_KEY_RELEASE;
    message->KeyCode = keyCode;

    return (lcGenericMessage_t *)message;
}

internal lcGenericMessage_t *
_lcMouseButtonPressMessageCreate(i32 keyCode)
{
    lcMouseButtonPressMessage_t *message;

    message = malloc(sizeof(lcMouseButtonPressMessage_t));
    message->Header.Type = LC_MESSAGE_TYPE_MOUSE_BUTTON_PRESS;
    message->KeyCode = keyCode;

    return (lcGenericMessage_t *)message;
}

internal lcGenericMessage_t *
_lcMouseButtonReleaseMessageCreate(i32 keyCode)
{
    lcMouseButtonReleaseMessage_t *message;

    message = malloc(sizeof(lcMouseButtonReleaseMessage_t));
    message->Header.Type = LC_MESSAGE_TYPE_MOUSE_BUTTON_RELEASE;
    message->KeyCode = keyCode;

    return (lcGenericMessage_t *)message;
}

internal lcGenericMessage_t *
_lcMouseScrollMessageCreate(i32 offset)
{
    lcMouseScrollMessage_t *message;

    message = malloc(sizeof(lcMouseScrollMessage_t));
    message->Header.Type = LC_MESSAGE_TYPE_MOUSE_SCROLL;
    message->ScrollOffset = offset;

    return (lcGenericMessage_t *)message;
}

void
lcMessageBind(i32 messageType,
              lcMessageHandler_t handler)
{
    LC_LIST_PUSH_BACK(_lcMessageHandlers[messageType], &handler);
}

internal void
_lcMessageEmit(lcGenericMessage_t *message)
{
    i32 i;

    for (i = 0; i < LC_LIST_LEN(_lcMessageHandlers[message->Type]); ++i)
    {
        (*_lcMessageHandlers[message->Type][i])(message);
    }
    free(message);
}

internal void
_lcMessageSystemDestroy(void)
{
    i32 i;
    for (i = 0; i < LC_MESSAGE_TYPE_COUNT; ++i)
    {
        LC_LIST_DESTROY(_lcMessageHandlers[i]);
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

