using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Auxiliar
{
    class Program
    {
        static void Main(string[] args)
        {
            DirectoryInfo di = new DirectoryInfo(@"E:\Google Drive\Mestrado\52 - Base de imagens\teste");
            FileInfo[] files = di.GetFiles("*.png");

            using (System.IO.StreamWriter file =
                new System.IO.StreamWriter(@"E:\Google Drive\Mestrado\52 - Base de imagens\teste\000-list.txt", true))
            {
                foreach (var fileInfo in files)
                {
                    file.WriteLine(fileInfo.FullName);
                }
            }
        }
    }
}
