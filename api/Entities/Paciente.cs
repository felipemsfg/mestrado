using System;
using System.Collections.Generic;
using System.Runtime.InteropServices.WindowsRuntime;
using MongoDB.Bson.Serialization.Attributes;
using MongoDB.Driver.Core.Operations;

namespace Mestrado.Entities
{
    [BsonIgnoreExtraElements]
    public class Paciente : IEntity
    {
        public string PatientId { get; set; }
        public string ImageDate { get; set; }
        public int MtaLeft { get; set; }
        public int MtaRight { get; set; }
        public string MtaRightImage { get; set; }
        public string MtaLeftImage { get; set; }
        public Guid Id { get; set; }
    }
}