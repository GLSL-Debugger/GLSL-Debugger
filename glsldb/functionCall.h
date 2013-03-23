/******************************************************************************

Copyright (C) 2006-2009 Institute for Visualization and Interactive Systems
(VIS), Universität Stuttgart.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice, this
	list of conditions and the following disclaimer in the documentation and/or
	other materials provided with the distribution.

  * Neither the name of the name of VIS, Universität Stuttgart nor the names
	of its contributors may be used to endorse or promote products derived from
	this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#ifndef _FUNCTION_CALL_H_
#define _FUNCTION_CALL_H_

class FunctionCall {
public:
    FunctionCall();
    FunctionCall(const FunctionCall *copyOf);
    ~FunctionCall();

    struct Argument {
        int   iType;
        void *pData;
        void *pAddress;
    };
    
    const char* getName(void) const;
    void setName(const char*);

    const char* getExtension(void) const;
    
    int getNumArguments(void) const;
    const Argument* getArgument(int) const;
    void addArgument(int, void*, void*);
    void editArgument(int, void*);

    bool operator==(const FunctionCall&);
    bool operator!=(const FunctionCall&);

    char* getCallString(void) const;
    
    bool isDebuggable(void) const;
    bool isDebuggable(int *primitiveMode) const;
    bool isEditable(void) const;
    bool isDebuggableDrawCall(void) const;
    bool isDebuggableDrawCall(int *primitiveMode) const;
    bool isShaderSwitch(void) const;
    bool isGlFunc(void) const;
    bool isGlxFunc(void) const;
	bool isWglFunc(void) const;
    bool isFrameEnd(void) const;
    bool isFramebufferChange(void) const;

private:
    char* getArgumentString(Argument arg) const;
    void* copyArgument(int type, void *addr);

    char     *m_pName;
    char     *m_pExtension;
    int       m_iNumArgs;
    Argument *m_pArguments;

};

#endif
