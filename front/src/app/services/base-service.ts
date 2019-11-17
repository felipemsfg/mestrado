import { Injectable } from '@angular/core';
import { HttpHeaders } from '@angular/common/http';
import { environment } from '../../environments/environment';

@Injectable({
  providedIn: 'root'
})
export class BaseService {
    protected UrlService: string;
    protected httpJsonOptions = {
        headers: new HttpHeaders({ 'Content-Type': 'application/json' })
    };
    protected env: any;

    constructor() {
        this.env = environment;
        this.UrlService = `http://localhost:5000/`;
    }

}
