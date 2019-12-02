using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Net.Mime;
using System.Text;
using System.Threading.Tasks;

namespace SplitImageFramework
{
    class Program
    {
        static void Main(string[] args)
        {
            ReadPngFromFolder(@"D:\Google Drive\Mestrado\52 - Base de imagens\working_directory", @"D:\Google Drive\Mestrado\52 - Base de imagens\destination_path");
        }

        private static void ReadPngFromFolder(string path, string destinationPath)
        {
            foreach (string file in Directory.EnumerateFiles(path, "*.png"))
            {
                SplitImages(file, destinationPath);
            }
        }

        private static void SplitImages(string imagePath, string destinationPath)
        {
            // Get the inputs.
            var bm = Image.FromFile(imagePath);
            var width = (int)(bm.Width / 2);
            var height = bm.Height;

            var pieceName = Path.GetFileNameWithoutExtension(imagePath);
            var piece = new Bitmap(width, height);

            // Left
            var destRect = new Rectangle(0, 0, width, height);
            using (var gr = Graphics.FromImage(piece))
            {
                var sourceRect = new Rectangle(0, 0, width, height);
                gr.DrawImage(bm, destRect, sourceRect, GraphicsUnit.Pixel);

                // Save the piece.
                var filename = $@"{destinationPath}\{pieceName} esquerdo.png";
                piece.Save(filename, ImageFormat.Png);
            }


            using (var gr = Graphics.FromImage(piece))
            {
                // Right
                var sourceRightRect = new Rectangle(bm.Width - width, 0, width, height);
                gr.DrawImage(bm, destRect, sourceRightRect, GraphicsUnit.Pixel);

                // Save the piece.
                var filename = $@"{destinationPath}\{pieceName} direito.png";
                piece.Save(filename, ImageFormat.Png);
            }
        }
    }
}
