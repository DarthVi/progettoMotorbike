//
// Created by vincenzo on 12/11/19.
//

class Streetlamp
{
public:
    float e[3];
    float n[3];
    void Init();
    Streetlamp()
    {
        Init();
    }
    void DrawStreetlamp(float posx, float posy, float posz);
};