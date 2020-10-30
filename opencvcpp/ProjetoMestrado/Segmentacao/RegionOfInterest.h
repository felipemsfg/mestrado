#pragma once
namespace Mestrado
{
    class ROI
    {
    private:
        static void CortarAreaDeInteresse(std::string str, std::string saveResultFolder, int alturaMedia);
    public:
        static void CriarImagesDaAreaDeInteresse(int alturaMedia);
    };

}
