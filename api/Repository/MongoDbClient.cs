using System;
using MongoDB.Driver;

namespace Mestrado.Repository
{
    public class MongoDbClient : IDisposable
    {
        public MongoClient Connection { get; }
        public IMongoDatabase Database { get; }
        public MongoDbClient(string connectionString, string databaseName)
        {
            Connection = new MongoClient(connectionString);
            Database = Connection.GetDatabase(databaseName);
        }

        public void Dispose()
        {

        }
    }
}