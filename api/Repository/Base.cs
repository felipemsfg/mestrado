using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Mestrado.Entities;
using MongoDB.Driver;

namespace Mestrado.Repository
{
    public class Base<TOne> : IBase<TOne> where TOne : class, IEntity
    {
        private MongoDbClient Client { get; }
        private IMongoCollection<TOne> Collection { get; }

        private string CollectionName { get; }

        public Base(MongoDbClient client, string collectionName)
        {
            CollectionName = collectionName;
            Client = client;
            Collection = Client.Database.GetCollection<TOne>(CollectionName);
        }

        public async Task<bool> AddOrReplace(TOne proposta)
        {
            try
            {
                var replaceOneResult = await Collection.ReplaceOneAsync(
                    doc => doc.Id.Equals(proposta.Id),
                    proposta,
                    new UpdateOptions { IsUpsert = true });

                return true;
            }
            catch
            {
                return false;
            }
        }

        public IEnumerable<TOne> GetAll()
        {
            return Collection.Find(f => true).ToList();
        }

        public TOne GetById(Guid id)
        {
            return Collection.Find(f => f.Id == id).FirstOrDefault<TOne>();
        }

        public void Delete(Guid id)
        {
            Collection.DeleteOne(f => f.Id == id);
        }
    }
}