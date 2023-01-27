//
// Created by behemot on 27.01.2023.
//

#ifndef PIUCNTVFD1_ANIMATORFAILURELISTENERINTERFACE_H
#define PIUCNTVFD1_ANIMATORFAILURELISTENERINTERFACE_H


class AnimatorFailureListenerInterface
{
public:
    static const short WARNING_UNDERTIME = 0;

    virtual void failureWarning(short warningCode) = 0;
};


#endif //PIUCNTVFD1_ANIMATORFAILURELISTENERINTERFACE_H
