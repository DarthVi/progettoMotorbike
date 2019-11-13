//
// Created by vincenzo on 13/11/19.
//

class Barile
{
public:
    float e[3];
    float n[3];
    void Init();
    Barile()
    {
        Init();
    }
    void DrawBarile(float posx, float posy, float posz);
};
