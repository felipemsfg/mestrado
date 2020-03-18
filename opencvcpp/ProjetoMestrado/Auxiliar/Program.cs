using System.IO;

namespace Auxiliar
{
    class Program
    {
        static void Main(string[] args)
        {
            const string driverLetter = "E";
            var di = new DirectoryInfo($@"{driverLetter}:\Code\felipemsfg.github.com\mestrado\opencvcpp\ProjetoMestrado\x64\Debug\imagens_sem_fundo");
            var files = di.GetFiles("*.png");

            using (var file =
                new StreamWriter($@"{driverLetter}:\Code\felipemsfg.github.com\mestrado\opencvcpp\ProjetoMestrado\x64\Debug\imagens\000-list.txt", true))
            {
                foreach (var fileInfo in files)
                {
                    file.WriteLine(fileInfo.FullName);
                }
            }
        }
    }
}
