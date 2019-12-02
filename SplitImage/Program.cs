using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;

namespace SplitImage
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
            var width = (int) (bm.Width / 2);
            var height = bm.Height;

            var pieceName = Path.GetFileNameWithoutExtension(imagePath);
            var piece = new Bitmap(width, height);

            // Left
            var destRect = new Rectangle(0, 0, width, height);
            using var gr = Graphics.FromImage(piece);
            var sourceRect = new Rectangle(0, 0, width, height);
            gr.DrawImage(bm, destRect, sourceRect, GraphicsUnit.Pixel);

            // Save the piece.
            var filename = $@"{destinationPath}\{pieceName} esquedo.png";
            piece.Save(filename, ImageFormat.Png);

            // Right
            using var grRight = Graphics.FromImage(piece);
            var sourceRightRect = new Rectangle(bm.Width - width, 0, width, height);
            gr.DrawImage(bm, destRect, sourceRect, GraphicsUnit.Pixel);

            // Save the piece.
            filename = $@"{destinationPath}\{pieceName} direito.png";
            piece.Save(filename, ImageFormat.Png);
        }
    }
}
